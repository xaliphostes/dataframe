/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include <array>
#include <dataframe/Serie.h>
#include <dataframe/common.h>
#include <type_traits>
#include <vector>

/**
 * @brief Meta programming for binary operations
 */

namespace df {
namespace details {

/**
 * @brief Generic element-wise operation implementation
 */
template <typename Op, typename T, typename U>
auto apply_op_elements_impl(const T &a, const U &b, Op op)
    -> decltype(op(a, b)) {
    return op(a, b);
}

/**
 * @brief Helper for fixed-size array operations
 */
template <typename Op, typename T, typename U, std::size_t N>
auto apply_op_array(const std::array<T, N> &a, const std::array<U, N> &b,
                    Op op) {
    using ResultType = decltype(apply_op_elements_impl(std::declval<T>(),
                                                       std::declval<U>(), op));
    details::check_default_constructible<ResultType>();

    std::array<ResultType, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = apply_op_elements_impl(a[i], b[i], op);
    }
    return result;
}

/**
 * @brief Helper for fixed-size container types
 */
template <typename Op, typename T, typename U>
auto apply_op_fixed(const T &a, const U &b, Op op) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Container sizes must match for operation");
    }

    T result;
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = apply_op_elements_impl(a[i], b[i], op);
    }
    return result;
}

/**
 * @brief Helper for resizable containers
 */
template <typename Op, typename T, typename U>
auto apply_op_resizable(const T &a, const U &b, Op op) {
    if (a.size() != b.size()) {
        throw std::runtime_error("Container sizes must match for operation");
    }

    using ResultType = decltype(apply_op_elements_impl(a[0], b[0], op));
    std::vector<ResultType> result;
    result.reserve(a.size());

    for (size_t i = 0; i < a.size(); ++i) {
        result.push_back(apply_op_elements_impl(a[i], b[i], op));
    }
    return result;
}

/**
 * @brief Main operation dispatcher
 */
template <typename Op, typename T, typename U>
auto apply_op(const T &a, const U &b, Op op) {
    if constexpr (is_container<T>::value && is_container<U>::value) {
        if constexpr (is_std_array<T>::value && is_std_array<U>::value &&
                      container_size<T>::value == container_size<U>::value) {
            return apply_op_array(a, b, op);
        } else if constexpr (std::is_same_v<T, U> &&
                             !is_resizable_container<T>::value) {
            return apply_op_fixed(a, b, op);
        } else {
            return apply_op_resizable(a, b, op);
        }
    } else {
        return apply_op_elements_impl(a, b, op);
    }
}

/**
 * @brief Operator type deduction helper
 */
template <typename Op, typename T, typename U>
using operation_result_t = decltype(apply_op(
    std::declval<T>(), std::declval<U>(), std::declval<Op>()));

template <typename Op> struct operation {
    template <typename T, typename U>
    auto operator()(const Serie<T> &serie1, const Serie<U> &serie2) const {
        details::check_default_constructible<T>();
        details::check_default_constructible<U>();

        if (serie1.size() != serie2.size()) {
            throw std::runtime_error("Series must have the same size");
        }

        return serie1.map([&serie2](const T &value, size_t i) {
            return details::apply_op(value, serie2[i], Op{});
        });
    }
};

// -----------------------------------------------------

/**
 * @brief Generic binder for any binary operation
 */
template <typename Op, typename U> struct binary_op_binder {
    const Serie<U> &serie2;

    explicit binary_op_binder(const Serie<U> &s) : serie2(s) {}

    template <typename T> auto operator()(const Serie<T> &serie1) const {
        // Op{} must be the operation function object
        return Op{}(serie1, serie2);
    }
};

/**
 * @brief Helper to create a binder
 */
template <typename Op, typename U>
auto make_binary_binder(const Serie<U> &serie2) {
    return binary_op_binder<Op, U>(serie2);
}

} // namespace details
} // namespace df
