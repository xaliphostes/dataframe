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

namespace df
{

    namespace detail
    {
        // Helper pour détecter le type de retour d'une fonction
        template <typename F>
        struct function_filter_traits : function_filter_traits<decltype(&F::operator())>
        {
        };

        template <typename C, typename R, typename... Args>
        struct function_filter_traits<R (C::*)(Args...) const>
        {
            using return_type = R;
            using args_tuple = std::tuple<Args...>;
        };

        // Type trait pour détecter les callbacks scalaires
        template <typename F>
        struct callback_filter_traits
        {
            using clean_type = std::remove_cv_t<std::remove_reference_t<F>>;
            using traits = function_filter_traits<clean_type>;
            using first_arg = std::tuple_element_t<0, typename traits::args_tuple>;

            static constexpr bool is_scalar = std::is_same_v<
                std::remove_cv_t<std::remove_reference_t<first_arg>>,
                double>;

            // Vérifie que le callback retourne bien un bool
            static constexpr bool returns_bool = std::is_same_v<
                typename traits::return_type,
                bool>;
        };

        template <typename F>
        inline constexpr bool is_filter_scalar_callback_v = callback_filter_traits<F>::is_scalar;
    }

    /**
     * @brief Filters elements from a Serie based on a predicate
     *
     * @example
     * ```cpp
     * // Scalar filtering
     * Serie s1(1, {1, 2, 3, 4, 5});
     * auto evens = filter(s1, [](double v, uint32_t) {
     *     return (int)v % 2 == 0;
     * });
     *
     * // Vector filtering
     * Serie s2(3, {1,2,3, 4,5,6, 7,8,9});
     * auto filtered = filter(s2, [](const Array& v, uint32_t) {
     *     return v[0] > 3;  // Keep vectors where first component > 3
     * });
     * ```
     */
    template <typename F>
    auto filter(const Serie &serie, F &&cb)
        -> std::enable_if_t<detail::is_filter_scalar_callback_v<F>, Serie>
    {
        static_assert(detail::callback_filter_traits<F>::returns_bool,
                      "Filter predicate must return bool");

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            if (cb(serie.template get<double>(i), i))
            {
                indices.push_back(i);
            }
        }

        Serie result(1, indices.size());
        for (uint32_t i = 0; i < indices.size(); ++i)
        {
            result.template set(i, serie.template get<double>(indices[i]));
        }
        return result;
    }

    template <typename F>
    auto filter(const Serie &serie, F &&cb)
        -> std::enable_if_t<!detail::is_filter_scalar_callback_v<F>, Serie>
    {
        static_assert(detail::callback_filter_traits<F>::returns_bool,
                      "Filter predicate must return bool");

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            if (cb(serie.template get<Array>(i), i))
            {
                indices.push_back(i);
            }
        }

        Serie result(serie.itemSize(), indices.size());
        for (uint32_t i = 0; i < indices.size(); ++i)
        {
            result.template set(i, serie.template get<Array>(indices[i]));
        }
        return result;
    }

    /**
     * @brief Creates a reusable filter function
     */
    template <typename F>
    auto makeFilter(F &&cb)
    {
        static_assert(detail::callback_filter_traits<F>::returns_bool,
                      "Filter predicate must return bool");

        return [cb = std::forward<F>(cb)](const auto &serie)
        {
            return filter(serie, cb);
        };
    }

    // template <typename F>
    // Serie filter(const Serie &serie, F &&predicate)
    // {
    //     return serie.filter(predicate);
    // }

}
