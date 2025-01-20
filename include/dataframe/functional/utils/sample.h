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
#include <stdexcept>
#include <random>

namespace df {
namespace utils {

/**
 * @brief Randomly samples n elements from a series
 * @example
 * ```cpp
 * GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6});
 * auto result = sample(3, s1);  // Randomly selects 3 elements
 *
 * // Using with pipe operator
 * auto result2 = s1 | make_sample(3);
 * ```
 * @param n Number of elements to sample
 * @param replacement Whether to sample with replacement (default: false)
 */
template <typename T>
GenSerie<T> sample(uint32_t n, const GenSerie<T> &serie,
                   bool replacement = false) {
    if (n > serie.count() && !replacement) {
        throw std::invalid_argument("Sample size cannot be larger than series "
                                    "count when sampling without replacement");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    if (replacement) {
        // Sampling with replacement
        std::uniform_int_distribution<uint32_t> dist(0, serie.count() - 1);
        GenSerie<T> result(serie.itemSize(), n);

        for (uint32_t i = 0; i < n; ++i) {
            uint32_t idx = dist(gen);
            if (serie.itemSize() == 1) {
                result.setValue(i, serie.value(idx));
            } else {
                result.setArray(i, serie.array(idx));
            }
        }
        return result;
    } else {
        // Sampling without replacement using Fisher-Yates shuffle
        std::vector<uint32_t> indices(serie.count());
        std::iota(indices.begin(), indices.end(), 0);

        for (uint32_t i = 0; i < n; ++i) {
            std::uniform_int_distribution<uint32_t> dist(i, serie.count() - 1);
            uint32_t j = dist(gen);
            std::swap(indices[i], indices[j]);
        }

        GenSerie<T> result(serie.itemSize(), n);
        for (uint32_t i = 0; i < n; ++i) {
            if (serie.itemSize() == 1) {
                result.setValue(i, serie.value(indices[i]));
            } else {
                result.setArray(i, serie.array(indices[i]));
            }
        }
        return result;
    }
}

/**
 * @brief Creates a sample operation that can be used with the pipe operator
 */
inline auto make_sample(uint32_t n, bool replacement = false) {
    return [=](const auto &serie) {
        using T = typename std::decay_t<decltype(serie)>::value_type;
        return sample<T>(n, serie, replacement);
    };
}

} // namespace utils
} // namespace df