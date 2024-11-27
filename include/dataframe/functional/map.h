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
#include <stdexcept>
#include <functional>
#include <type_traits>

namespace df
{

    namespace detail
    {
        template <typename F>
        struct callback_traits
        {
        private:
            template <typename G>
            static auto test_input(int)
                -> decltype(std::declval<G>()(std::declval<double>(), std::declval<uint32_t>()), std::true_type{});

            template <typename>
            static auto test_input(...) -> std::false_type;

            template <typename G>
            static auto test_return(int)
                -> std::is_same<decltype(std::declval<G>()(
                                    std::declval<double>(), std::declval<uint32_t>())),
                                double>;

            template <typename>
            static auto test_return(...) -> std::false_type;

        public:
            static constexpr bool has_scalar_input = decltype(test_input<F>(0))::value;
            static constexpr bool has_scalar_output = decltype(test_return<F>(0))::value;
        };

        template <typename F>
        inline constexpr bool has_scalar_input_v = callback_traits<F>::has_scalar_input;

        template <typename F>
        inline constexpr bool has_scalar_output_v = callback_traits<F>::has_scalar_output;
    }

    /**
     * @brief Maps a function over a Serie, allowing change in itemSize
     */
    template <typename F>
    auto map(const Serie &serie, F &&cb)
    {
        if constexpr (detail::has_scalar_input_v<F>)
        {
            // Input is scalar
            using ResultType = std::invoke_result_t<F, double, uint32_t>;

            if constexpr (std::is_arithmetic_v<ResultType>)
            {
                // Scalar to scalar (including int, double, etc.)
                Array results;
                results.reserve(serie.count());
                for (uint32_t i = 0; i < serie.count(); ++i)
                {
                    results.push_back(static_cast<double>(cb(serie.template get<double>(i), i)));
                }
                return Serie(1, results);
            }
            else
            {
                // Scalar to vector
                if (serie.count() == 0)
                {
                    return Serie(0, 0);
                }
                auto first_result = cb(serie.template get<double>(0), 0);
                size_t itemSize = first_result.size();

                Array flattened;
                flattened.reserve(itemSize * serie.count());
                flattened.insert(flattened.end(), first_result.begin(), first_result.end());

                for (uint32_t i = 1; i < serie.count(); ++i)
                {
                    auto result = cb(serie.template get<double>(i), i);
                    if (result.size() != itemSize)
                    {
                        throw std::runtime_error("Inconsistent output vector sizes in map");
                    }
                    flattened.insert(flattened.end(), result.begin(), result.end());
                }
                return Serie(itemSize, flattened);
            }
        }
        else
        {
            // Input is vector
            using ResultType = std::invoke_result_t<F, const Array &, uint32_t>;

            if constexpr (std::is_same_v<ResultType, double>)
            {
                // Vector to scalar
                Array results;
                results.reserve(serie.count());
                for (uint32_t i = 0; i < serie.count(); ++i)
                {
                    results.push_back(cb(serie.template get<Array>(i), i));
                }
                return Serie(1, results);
            }
            else
            {
                // Vector to vector
                if (serie.count() == 0)
                {
                    return Serie(0, 0);
                }
                auto first_result = cb(serie.template get<Array>(0), 0);
                size_t itemSize = first_result.size();

                Array flattened;
                flattened.reserve(itemSize * serie.count());
                flattened.insert(flattened.end(), first_result.begin(), first_result.end());

                for (uint32_t i = 1; i < serie.count(); ++i)
                {
                    auto result = cb(serie.template get<Array>(i), i);
                    if (result.size() != itemSize)
                    {
                        throw std::runtime_error("Inconsistent output vector sizes in map");
                    }
                    flattened.insert(flattened.end(), result.begin(), result.end());
                }
                return Serie(itemSize, flattened);
            }
        }
    }

    /**
     * @brief Creates a reusable map function
     */
    MAKE_OP(map);

} // namespace df
