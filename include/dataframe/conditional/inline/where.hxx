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
 *
 */

namespace df {
namespace details {

// Helper for condition evaluation on arithmetic types
template <typename T>
bool evaluate_condition_impl(const T &value, std::true_type) {
    return static_cast<bool>(value);
}

// Helper for array/container types
template <typename T, std::size_t N>
bool evaluate_array_condition(const std::array<T, N> &arr) {
    for (const auto &val : arr) {
        if (val != T{})
            return true;
    }
    return false;
}

// Helper for container types
template <typename T>
bool evaluate_condition_impl(const T &value, std::false_type) {
    if constexpr (is_std_array<T>::value) {
        return evaluate_array_condition(value);
    } else if constexpr (is_container<T>::value) {
        for (const auto &val : value) {
            if (val != typename container_value_type<T>::type{})
                return true;
        }
        return false;
    } else {
        static_assert(
            std::is_arithmetic<T>::value || is_container<T>::value,
            "Condition type must be either arithmetic or a container type");
        return false; // Unreachable
    }
}

// Main dispatch function for condition evaluation
template <typename T> bool evaluate_condition(const T &value) {
    return evaluate_condition_impl(value, std::is_arithmetic<T>{});
}

} // namespace details

// Main where function
template <typename CondT, typename ThenT, typename ElseT>
auto where(const Serie<CondT> &condition, const Serie<ThenT> &then_serie,
           const Serie<ElseT> &else_serie) {
    if (condition.size() != then_serie.size() ||
        condition.size() != else_serie.size()) {
        throw std::runtime_error("All series must have the same size in where");
    }

    using ResultType =
        std::decay_t<decltype(details::evaluate_condition(std::declval<CondT>())
                                  ? then_serie[0]
                                  : else_serie[0])>;

    std::vector<ResultType> result;
    result.reserve(condition.size());

    for (size_t i = 0; i < condition.size(); ++i) {
        result.push_back(details::evaluate_condition(condition[i])
                             ? ResultType(then_serie[i])
                             : ResultType(else_serie[i]));
    }

    return Serie<ResultType>(std::move(result));
}

// Overload for scalar then/else values
template <typename CondT, typename ThenT, typename ElseT>
auto where(const Serie<CondT> &condition, const ThenT &then_value,
           const ElseT &else_value) {
    using ResultType =
        std::decay_t<decltype(details::evaluate_condition(std::declval<CondT>())
                                  ? then_value
                                  : else_value)>;

    std::vector<ResultType> result;
    result.reserve(condition.size());

    for (size_t i = 0; i < condition.size(); ++i) {
        result.push_back(details::evaluate_condition(condition[i])
                             ? ResultType(then_value)
                             : ResultType(else_value));
    }

    return Serie<ResultType>(std::move(result));
}

// Pipeline support
template <typename ThenT, typename ElseT> struct where_binder {
    const Serie<ThenT> &then_serie;
    const Serie<ElseT> &else_serie;

    where_binder(const Serie<ThenT> &t, const Serie<ElseT> &e)
        : then_serie(t), else_serie(e) {}

    template <typename CondT>
    auto operator()(const Serie<CondT> &condition) const {
        return where(condition, then_serie, else_serie);
    }
};

template <typename ThenT, typename ElseT> struct where_scalar_binder {
    const ThenT then_value;
    const ElseT else_value;

    where_scalar_binder(const ThenT &t, const ElseT &e)
        : then_value(t), else_value(e) {}

    template <typename CondT>
    auto operator()(const Serie<CondT> &condition) const {
        return where(condition, then_value, else_value);
    }
};

// Helper functions to create binders
template <typename ThenT, typename ElseT>
auto bind_where(const Serie<ThenT> &then_serie,
                const Serie<ElseT> &else_serie) {
    return where_binder<ThenT, ElseT>(then_serie, else_serie);
}

template <typename ThenT, typename ElseT>
auto bind_where(const ThenT &then_value, const ElseT &else_value) {
    return where_scalar_binder<ThenT, ElseT>(then_value, else_value);
}

} // namespace df