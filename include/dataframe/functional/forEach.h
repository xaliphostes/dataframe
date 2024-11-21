/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <type_traits>

namespace df
{

    namespace detail
    {

        // Type trait to detect array callback
        template <typename F, typename = void>
        struct is_array_callback : std::false_type
        {
        };

        template <typename F>
        struct is_array_callback<F, std::void_t<decltype(std::declval<F>()(
                                        std::declval<const Array &>(), std::declval<uint32_t>()))>> : std::true_type
        {
        };

        template <typename F>
        inline constexpr bool is_array_callback_v = is_array_callback<F>::value;
    }

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
    template <typename Serie, typename F>
    void forEach(const Serie &serie, F &&cb)
    {
        static_assert(detail::is_scalar_callback_v<F> || detail::is_array_callback_v<F>,
                      "Callback must accept either (double, uint32_t) or (const Array&, uint32_t)");

        if constexpr (detail::is_scalar_callback_v<F>)
        {
            // Scalar version
            for (uint32_t i = 0; i < serie.count(); ++i)
            {
                cb(serie.template get<double>(i), i);
            }
        }
        else
        {
            // Array version
            for (uint32_t i = 0; i < serie.count(); ++i)
            {
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
     * auto printer = makeForEach([](const auto& v, uint32_t i) {
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
    template <typename F>
    auto makeForEach(F &&cb)
    {
        return [cb = std::forward<F>(cb)](const auto &serie)
        {
            forEach(serie, cb);
        };
    }

    /**
     * @brief Indexed forEach that provides more index information
     * @param serie Input Serie
     * @param cb Callback with additional index information:
     *          (value, index, isFirst, isLast)
     *
     * @example
     * ```cpp
     * Serie s(1, {1, 2, 3});
     * forEachIndexed(s, [](const auto& v, uint32_t i, bool isFirst, bool isLast) {
     *     std::cout << "Item " << i
     *               << (isFirst ? " (first)" : "")
     *               << (isLast ? " (last)" : "")
     *               << ": " << v << "\n";
     * });
     * ```
     */
    template <typename Serie, typename F>
    void forEachIndexed(const Serie &serie, F &&cb)
    {
        const uint32_t count = serie.count();
        if (count == 0)
            return;

        if constexpr (detail::is_scalar_callback_v<F>)
        {
            // Scalar version
            for (uint32_t i = 0; i < count; ++i)
            {
                cb(serie.template get<double>(i), i, i == 0, i == count - 1);
            }
        }
        else
        {
            // Array version
            for (uint32_t i = 0; i < count; ++i)
            {
                cb(serie.template get<Array>(i), i, i == 0, i == count - 1);
            }
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
//             std::for_each(std::execution::par_unseq, indices.begin(), indices.end(),
//                           [&](uint32_t i)
//                           {
//                               cb(serie.template get<double>(i), i);
//                           });
//         }
//         else
//         {
//             // Array version
//             std::for_each(std::execution::par_unseq, indices.begin(), indices.end(),
//                           [&](uint32_t i)
//                           {
//                               cb(serie.template get<Array>(i), i);
//                           });
//         }
//     }
// #endif

}
