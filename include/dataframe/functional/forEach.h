/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/functional/macros.h>
#include <dataframe/utils.h>

#include <type_traits>

namespace df {

/** @brief Generic forEach function for Series
 * @param cb Callback function with signature either:
 *          - (const Array& value, uint32_t index) for non-scalar
 *          - (double value, uint32_t index) for scalar
 * @param args The list of Serie arguments or just one Serie

 *
 * @example
 * ```cpp
 * // For scalar Serie
 * Serie s1(1, {1, 2, 3});
 * forEach([](double v, uint32_t i) {
 *     std::cout << "Value at " << i << ": " << v << "\n";
 * }, s1);
 *
 * // For non-scalar Serie
 * Serie s2(3, {1,2,3, 4,5,6});
 * forEach([](const Array& v, uint32_t i) {
 *     std::cout << "Vector at " << i << ": " << v << "\n";
 * }, s2);
 *
 * // Using mulitple series
 * df::forEach(
 *     [=](const Array &v1, const Array &v2, uint32_t i) {
 *         std::cout << "Index " << i << ":\n"
 *         << "  v1 = " << v1 << ", v2 = " << v2 << std::endl;
 *     },
 *     s1, s2);
 * ```
 */
template <typename F, typename... Args>
void forEach(F &&callback, const Args &...args) {
    if constexpr (details::is_multi_series_call<Args...>::value) {
        // Multiple Series case (forEachAll)
        static_assert(std::conjunction<details::is_serie<Args>...>::value,
                      "All arguments after callback must be Series");

        auto counts = utils::countAndCheck(args...);

        // Iterate over all series simultaneously
        for (uint32_t i = 0; i < counts[0]; ++i) {
            callback(args.template get<Array>(i)..., i);
        }
    } else {
        // Single Serie case (original forEach)
        static_assert(sizeof...(args) == 1,
                      "Single Serie forEach requires exactly one Serie");
        static_assert(details::is_scalar_callback_v<F> ||
                          details::is_array_callback_v<F> ||
                          details::is_generic_callback_v<F>,
                      "Callback must accept either (double, uint32_t), (const "
                      "Array&, uint32_t), or be generic");

        const auto &serie = std::get<0>(std::forward_as_tuple(args...));

        if constexpr (details::is_generic_callback_v<F>) {
            for (uint32_t i = 0; i < serie.count(); ++i) {
                if (serie.itemSize() == 1) {
                    callback(serie.template get<double>(i), i);
                } else {
                    callback(serie.template get<Array>(i), i);
                }
            }
        } else if constexpr (details::is_scalar_callback_v<F>) {
            if (serie.itemSize() != 1) {
                throw std::invalid_argument(
                    "Cannot use scalar callback for Serie with itemSize > 1");
            }
            for (uint32_t i = 0; i < serie.count(); ++i) {
                callback(serie.template get<double>(i), i);
            }
        } else {
            if (serie.itemSize() == 1) {
                throw std::invalid_argument(
                    "Cannot use array callback for Serie with itemSize == 1");
            }
            for (uint32_t i = 0; i < serie.count(); ++i) {
                callback(serie.template get<Array>(i), i);
            }
        }
    }
}

/**
 * @brief Creates a reusable forEach function
 * @param cb Callback function
 * @return Function that takes a Serie and applies the callback
 *
 * @example
 * ```cpp
 * auto printer = make_forEach([](const auto& v, uint32_t i) {
 *     std::cout << "Item " << i << ": " << v << "\n";
 * });
 *
 * Serie s1(1, {1, 2, 3});
 * Serie s2(3, {1,2,3, 4,5,6});
 *
 * printer(s1); // Works with scalar Serie
 * printer(s2); // Works with non-scalar Serie
 * ```
 */
MAKE_OP(forEach);

} // namespace df