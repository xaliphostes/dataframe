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

namespace df
{

    /**
     * @brief Maps a function over a Serie's non-scalar values
     * @param serie Input Serie
     * @param f Function to apply to each value
     * @return Serie A new Serie containing mapped values
     * @throws std::invalid_argument if input Serie is invalid or is scalar
     *
     * @example
     * ```cpp
     * Serie s(3, {1,2,3, 4,5,6});  // 2 items of size 3
     * auto result = map(s, [](const Array& v, uint32_t i) {
     *     Array out(v.size());
     *     for(size_t j = 0; j < v.size(); ++j) {
     *         out[j] = v[j] * 2;
     *     }
     *     return out;
     * });
     * ```
     */
    template <typename F>
    Serie map(const Serie &serie, F &&f)
    {
        if (!serie.isValid())
        {
            throw std::invalid_argument("Input Serie must be valid");
        }

        if (serie.itemSize() < 1)
        {
            throw std::invalid_argument("Input Serie must have positive itemSize");
        }

        // Create result Serie with same dimensions
        Serie result(serie.itemSize(), serie.count(), serie.dimension());

        // Map each value
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            Array value = serie.value(i);
            Array mapped = f(value, i);

            if (mapped.size() != serie.itemSize())
            {
                throw std::invalid_argument(
                    "Mapped value size (" + std::to_string(mapped.size()) +
                    ") must match input Serie itemSize (" + std::to_string(serie.itemSize()) + ")");
            }

            result.setValue(i, mapped);
        }

        return result;
    }

    /**
     * @brief Maps a function over a Serie's scalar values
     * @param serie Input Serie
     * @param f Function to apply to each scalar value
     * @return Serie A new Serie containing mapped values
     * @throws std::invalid_argument if input Serie is invalid
     *
     * @example
     * ```cpp
     * Serie s(1, {1, 2, 3, 4});
     * auto result = mapScalar(s, [](double v, uint32_t i) {
     *     return v * 2;
     * });
     * ```
     */
    template <typename F>
    Serie mapScalar(const Serie &serie, F &&f)
    {
        if (!serie.isValid())
        {
            throw std::invalid_argument("Input Serie must be valid");
        }

        // Create result Serie with same dimensions
        Serie result(1, serie.count(), serie.dimension());

        // Map each scalar value
        for (uint32_t i = 0; i < serie.count(); ++i)
        {
            result.setScalar(i, f(serie.scalar(i), i));
        }

        return result;
    }

    /**
     * @brief Creates a mapping function that can be reused with different Series
     * @param f Function to apply to each value
     * @return Function that takes a Serie and returns a mapped Serie
     *
     * @example
     * ```cpp
     * auto doubler = makeMap([](const Array& v, uint32_t) {
     *     Array out(v.size());
     *     for(size_t j = 0; j < v.size(); ++j) {
     *         out[j] = v[j] * 2;
     *     }
     *     return out;
     * });
     *
     * Serie s1(3, {1,2,3, 4,5,6});
     * Serie s2(3, {7,8,9, 10,11,12});
     *
     * auto result1 = doubler(s1);
     * auto result2 = doubler(s2);
     * ```
     */
    template <typename F>
    auto makeMap(F &&f)
    {
        return [f = std::forward<F>(f)](const Serie &serie)
        {
            return map(serie, f);
        };
    }

    /**
     * @brief Creates a scalar mapping function that can be reused with different Series
     * @param f Function to apply to each scalar value
     * @return Function that takes a Serie and returns a mapped Serie
     *
     * @example
     * ```cpp
     * auto doubler = makeMapScalar([](double v, uint32_t) {
     *     return v * 2;
     * });
     *
     * Serie s1(1, {1, 2, 3});
     * Serie s2(1, {4, 5, 6});
     *
     * auto result1 = doubler(s1);
     * auto result2 = doubler(s2);
     * ```
     */
    template <typename F>
    auto makeMapScalar(F &&f)
    {
        return [f = std::forward<F>(f)](const Serie &serie)
        {
            return mapScalar(serie, f);
        };
    }

} // namespace df