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
        // Helper pour detecter le type de retour d'une fonction
        template <typename F>
        struct function_reduce_traits : function_reduce_traits<decltype(&F::operator())>
        {
        };

        template <typename C, typename R, typename... Args>
        struct function_reduce_traits<R (C::*)(Args...) const>
        {
            using return_type = R;
            using args_tuple = std::tuple<Args...>;
        };

        // Type trait pour détecter les callbacks scalaires
        template <typename F>
        struct callback_reduce_traits
        {
            using clean_type = std::remove_cv_t<std::remove_reference_t<F>>;
            using traits = function_reduce_traits<clean_type>;
            using first_arg = std::tuple_element_t<0, typename traits::args_tuple>;

            static constexpr bool is_scalar = std::is_same_v<
                std::remove_cv_t<std::remove_reference_t<first_arg>>,
                double>;
        };

        template <typename F>
        inline constexpr bool is_reduce_scalar_callback_v = callback_reduce_traits<F>::is_scalar;
    }

    /**
     * @brief Reduces a Serie to either a double or a Serie
     * @param serie Input Serie
     * @param cb Reducer function
     * @param init Initial value (double or Array)
     * @return double for scalar reduction, Serie for vector reduction
     *
     * @example
     * ```cpp
     * // Scalar reduction (returns double)
     * Serie s1(1, {1, 2, 3, 4, 5});
     * double sum = reduce(s1, [](double acc, double v, uint32_t) {
     *     return acc + v;
     * }, 0.0);
     *
     * // Vector reduction (returns Serie)
     * Serie s2(3, {1,2,3, 4,5,6});
     * Serie sum = reduce(s2, [](const Array& acc, const Array& v, uint32_t) {
     *     Array result(acc.size());
     *     for(size_t i = 0; i < acc.size(); ++i) result[i] = acc[i] + v[i];
     *     return result;
     * }, Array(3, 0.0));
     * ```
     */
    template <typename F>
    auto reduce(const Serie &serie, F &&cb, double init)
        -> std::enable_if_t<detail::is_reduce_scalar_callback_v<F>, double>
    {
        double result = init;
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            result = cb(result, serie.template get<double>(i), i);
        }
        return result;
    }

    template <typename F>
    auto reduce(const Serie &serie, F &&cb, const Array &init)
        -> std::enable_if_t<!detail::is_reduce_scalar_callback_v<F>, Serie>
    {
        Array result = init;
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            result = cb(result, serie.template get<Array>(i), i);
        }
        return Serie(result.size(), {result.begin(), result.end()});
    }

    /**
     * @brief Creates a reusable reduce function
     */
    template <typename F>
    auto make_reduce(F &&cb, double init)
    {
        static_assert(detail::is_reduce_scalar_callback_v<F>,
                      "Scalar reduce requires a callback taking doubles");
        return [cb = std::forward<F>(cb), init](const auto &serie)
        {
            return reduce(serie, cb, init);
        };
    }

    template <typename F>
    auto make_reduce(F &&cb, const Array &init)
    {
        static_assert(!detail::is_reduce_scalar_callback_v<F>,
                      "Vector reduce requires a callback taking Arrays");
        return [cb = std::forward<F>(cb), init](const auto &serie)
        {
            return reduce(serie, cb, init);
        };
    }

}
