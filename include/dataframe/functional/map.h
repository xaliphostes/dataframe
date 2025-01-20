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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/functional/common.h>
#include <dataframe/functional/macros.h>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace df {

/**
 * @brief Apply a function to each element of a serie
 * @code
 * // Create a map operation that squares each value
 * auto square = df::make_map([](double x, uint32_t) { return x * x; });
 * 
 * // Use it on different series
 * GenSerie<double> s1(1, {1, 2, 3, 4});
 * auto squared = square(s1);  // Returns a new series with squared values
 * 
 * // Create a map operation that combines two series
 * auto add = df::make_map([](double x, double y, uint32_t) { return x + y; });
 * GenSerie<double> s2(1, {5, 6, 7, 8});
 * auto sum = add(s1, s2);  // Returns a new series with sum of elements
 * @endcode
 */
template <typename F, typename T>
auto map(F &&callback, const GenSerie<T> &serie) {
    // Check what kind of input the callback expects
    using IsVectorInput = std::is_invocable<F, std::vector<T>, uint32_t>;
    using IsScalarInput = std::is_invocable<F, T, uint32_t>;

    // Handle scalar input
    if constexpr (IsScalarInput::value) {
        using ResultType = std::invoke_result_t<F, T, uint32_t>;

        // Case 1: Scalar input → Vector output
        if constexpr (details::is_array_v<ResultType>) {
            using ElementType = typename ResultType::value_type;
            auto first_result = callback(serie.value(0), 0);

            GenSerie<ElementType> result(first_result.size(), serie.count());
            result.setArray(0, first_result);

            for (uint32_t i = 1; i < serie.count(); ++i) {
                result.setArray(i, callback(serie.value(i), i));
            }
            return result;
        }
        // Case 2: For vector serie with scalar operations
        else if (serie.itemSize() > 1) {
            GenSerie<T> result(serie.itemSize(), serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i) {
                auto arr = serie.array(i);
                std::vector<T> transformed_arr(arr.size());
                for (size_t j = 0; j < arr.size(); ++j) {
                    transformed_arr[j] = callback(arr[j], i);
                }
                result.setArray(i, transformed_arr);
            }
            return result;
        }
        // Case 3: Scalar input → Scalar output
        else {
            GenSerie<ResultType> result(1, serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i) {
                result.setValue(i, callback(serie.value(i), i));
            }
            return result;
        }
    }
    // Handle vector input
    else if constexpr (IsVectorInput::value) {
        using ResultType = std::invoke_result_t<F, std::vector<T>, uint32_t>;

        // Case 4: Vector input → Scalar output
        if constexpr (std::is_scalar_v<ResultType>) {
            GenSerie<ResultType> result(1, serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i) {
                result.setValue(i, callback(serie.array(i), i));
            }
            return result;
        }
        // Case 5: Vector input → Vector output
        else {
            using ElementType = typename ResultType::value_type;
            auto first_result = callback(serie.array(0), 0);

            GenSerie<ElementType> result(first_result.size(), serie.count());
            result.setArray(0, first_result);

            for (uint32_t i = 1; i < serie.count(); ++i) {
                result.setArray(i, callback(serie.array(i), i));
            }
            return result;
        }
    } else {
        static_assert(IsScalarInput::value || IsVectorInput::value,
                      "Callback must accept either scalar or vector input");
    }
}

// Multi-series map
template <typename F, typename T, typename... Series>
auto map(F &&callback, const GenSerie<T> &first, const Series &...rest) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type");

    // Check counts match
    if (!((rest.count() == first.count()) && ...)) {
        throw std::invalid_argument("All series must have the same count");
    }

    // Check if callback accepts vectors or scalars
    using IsVectorInput =
        std::is_invocable<F, std::vector<T>, decltype(rest.array(0))...,
                          uint32_t>;
    using IsScalarInput =
        std::is_invocable<F, T, decltype(rest.value(0))..., uint32_t>;

    if constexpr (IsScalarInput::value) {
        using ResultType =
            std::invoke_result_t<F, T, decltype(rest.value(0))..., uint32_t>;

        // Handle vector result from scalar inputs
        if constexpr (details::is_array_v<ResultType>) {
            using ElementType = typename ResultType::value_type;
            auto first_result = callback(first.value(0), rest.value(0)..., 0);

            GenSerie<ElementType> result(first_result.size(), first.count());
            result.setArray(0, first_result);

            for (uint32_t i = 1; i < first.count(); ++i) {
                result.setArray(i,
                                callback(first.value(i), rest.value(i)..., i));
            }
            return result;
        }
        // Handle scalar result
        else {
            GenSerie<ResultType> result(1, first.count());
            for (uint32_t i = 0; i < first.count(); ++i) {
                result.setValue(i,
                                callback(first.value(i), rest.value(i)..., i));
            }
            return result;
        }
    } else if constexpr (IsVectorInput::value) {
        using ResultType =
            std::invoke_result_t<F, std::vector<T>, decltype(rest.array(0))...,
                                 uint32_t>;

        if constexpr (std::is_scalar_v<ResultType>) {
            GenSerie<ResultType> result(1, first.count());
            for (uint32_t i = 0; i < first.count(); ++i) {
                result.setValue(i,
                                callback(first.array(i), rest.array(i)..., i));
            }
            return result;
        } else {
            using ElementType = typename ResultType::value_type;
            auto first_result = callback(first.array(0), rest.array(0)..., 0);

            GenSerie<ElementType> result(first_result.size(), first.count());
            result.setArray(0, first_result);

            for (uint32_t i = 1; i < first.count(); ++i) {
                result.setArray(i,
                                callback(first.array(i), rest.array(i)..., i));
            }
            return result;
        }
    } else {
        static_assert(IsScalarInput::value || IsVectorInput::value,
                      "Callback must accept either scalar or vector inputs");
    }
}

// MAKE_OP(map);
namespace details {

template <typename F> class MapOp {
  public:
    explicit MapOp(F func) : func_(std::move(func)) {}
    template <typename T> auto operator()(const GenSerie<T> &serie) const {
        return map(func_, serie);
    }
    template <typename T, typename... Series>
    auto operator()(const GenSerie<T> &first, const Series &...rest) const {
        return map(func_, first, rest...);
    }

  private:
    F func_;
};

} // namespace details

template <typename F> auto make_map(F &&func) {
    return details::MapOp<std::decay_t<F>>(std::forward<F>(func));
}

} // namespace df
