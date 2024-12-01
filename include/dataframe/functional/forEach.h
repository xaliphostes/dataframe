/*
 * Copyright (c) 2023 fmaerten@gmail.com
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

#include <type_traits>

namespace df {

/**
 * @brief Generic forEach function for Series
 * @param serie Input Serie
 * @param cb Callback function with signature either:
 *          - (const Array& value, uint32_t index) for non-scalar
 *          - (double value, uint32_t index) for scalar
 *
 * @example
 * ```cpp
 * // For scalar Serie
 * Serie s1(1, {1, 2, 3});
 * forEach(s1, [](double v, uint32_t i) {
 *     std::cout << "Value at " << i << ": " << v << "\n";
 * });
 *
 * // For non-scalar Serie
 * Serie s2(3, {1,2,3, 4,5,6});
 * forEach(s2, [](const Array& v, uint32_t i) {
 *     std::cout << "Vector at " << i << ": " << v << "\n";
 * });
 *
 * // With auto type deduction
 * forEach(s2, [](const auto& v, uint32_t i) {
 *     std::cout << "Item " << i << ": " << v << "\n";
 * });
 * ```
 */
template <typename Serie, typename F> void forEach(F &&cb, const Serie &serie) {
    static_assert(details::is_scalar_callback_v<F> ||
                      details::is_array_callback_v<F> ||
                      details::is_generic_callback_v<F>,
                  "Callback must accept either (double, uint32_t), (const "
                  "Array&, uint32_t), or be generic");

    if constexpr (details::is_generic_callback_v<F>) {
        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (serie.itemSize() == 1) {
                cb(serie.template get<double>(i), i);
            } else {
                cb(serie.template get<Array>(i), i);
            }
        }
    } else if constexpr (details::is_scalar_callback_v<F>) {
        if (serie.itemSize() != 1) {
            throw std::invalid_argument(
                "Cannot use scalar callback for Serie with itemSize > 1");
        }
        for (uint32_t i = 0; i < serie.count(); ++i) {
            cb(serie.template get<double>(i), i);
        }
    } else {
        if (serie.itemSize() == 1) {
            throw std::invalid_argument(
                "Cannot use array callback for Serie with itemSize == 1");
        }
        for (uint32_t i = 0; i < serie.count(); ++i) {
            cb(serie.template get<Array>(i), i);
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

// ----------------------------------------------------------------

/**
 * @brief Apply a function to corresponding elements of multiple Series
 * @param callback Function taking n Arrays (or doubles) and index
 * @param series Variable number of Series to iterate over
 */
template <typename Callback, typename... Series>
void forEachAll(Callback &&callback, const Series &...series) {
    static_assert(sizeof...(series) >= 2,
                  "forEachAll requires at least 2 Series");
    static_assert(std::conjunction<details::is_serie<Series>...>::value,
                  "All arguments after callback must be Series");

    // Check counts match
    std::array<size_t, sizeof...(series)> counts = {
        details::get_count(series)...};
    for (size_t i = 1; i < counts.size(); ++i) {
        if (counts[i] != counts[0]) {
            throw std::invalid_argument("All Series must have the same count");
        }
    }

    // Iterate over all series simultaneously
    for (uint32_t i = 0; i < counts[0]; ++i) {
        callback(series.template get<Array>(i)..., i);
    }
}

// #ifndef __APPLE__
//     /**
//      * @brief Parallel forEach implementation
//      * @param serie Input Serie
//      * @param cb Callback function
//      *
//      * @note Requires C++17 or later
//      *
//      * @example
//      * ```cpp
//      * Serie s(3, {1,2,3, 4,5,6, 7,8,9});
//      * forEachParallel(s, [](const auto& v, uint32_t i) {
//      *     // This will be executed in parallel
//      *     heavyComputation(v);
//      * });
//      * ```
//      */
//     template <typename Serie, typename F>
//     void forEachParallel(const Serie &serie, F &&cb)
//     {
// #include <execution>
//         std::vector<uint32_t> indices(serie.count());
//         std::iota(indices.begin(), indices.end(), 0);

//         if constexpr (detail::is_scalar_callback_v<F>)
//         {
//             // Scalar version
//             std::for_each(std::execution::par_unseq, indices.begin(),
//             indices.end(),
//                           [&](uint32_t i)
//                           {
//                               cb(serie.template get<double>(i), i);
//                           });
//         }
//         else
//         {
//             // Array version
//             std::for_each(std::execution::par_unseq, indices.begin(),
//             indices.end(),
//                           [&](uint32_t i)
//                           {
//                               cb(serie.template get<Array>(i), i);
//                           });
//         }
//     }
// #endif

} // namespace df

namespace df {

/**
 * @brief Unified forEach that handles both single and multiple Series
 */
template <typename F, typename... Args>
void _forEach(F &&callback, const Args &...args) {
    if constexpr (details::is_multi_series_call<Args...>::value) {
        // Multiple Series case (forEachAll)
        static_assert(std::conjunction<details::is_serie<Args>...>::value,
                      "All arguments after callback must be Series");

        // Check counts match
        std::array<size_t, sizeof...(args)> counts = {
            details::get_count(args)...};
        for (size_t i = 1; i < counts.size(); ++i) {
            if (counts[i] != counts[0]) {
                throw std::invalid_argument(
                    "All Series must have the same count");
            }
        }

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

} // namespace df