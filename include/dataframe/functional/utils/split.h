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

namespace df {
namespace utils {

/**
 * @brief Split a GenSerie into n sub-series of approximately equal size
 * @example
 * ```cpp
 * GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
 * auto parts = split(3, s1); // vector<GenSerie<double>>
 * // parts[0]: {1,2,3,4}
 * // parts[1]: {5,6,7}
 * // parts[2]: {8,9,10}
 * ```
 * @ingroup Utils
 */
template <typename T>
std::vector<GenSerie<T>> split(uint32_t n, const GenSerie<T> &serie) {
    if (n == 0) {
        throw std::invalid_argument("n must be > 0");
    }

    uint32_t total = serie.count();
    uint32_t base_size = total / n;
    uint32_t remainder = total % n;

    std::vector<GenSerie<T>> partitions;
    partitions.reserve(n);

    uint32_t start = 0;
    for (uint32_t i = 0; i < n && start < total; ++i) {
        uint32_t size = base_size + (i < remainder ? 1 : 0);
        GenSerie<T> part(serie.itemSize(), size);

        for (uint32_t j = 0; j < size; ++j) {
            if (serie.itemSize() == 1) {
                part.setValue(j, serie.value(start + j));
            } else {
                part.setArray(j, serie.array(start + j));
            }
        }
        start += size;
        partitions.push_back(std::move(part));
    }

    return partitions;
}

/**
 * @brief Split multiple GenSeries into n parts each
 * @example
 * ```cpp
 * GenSerie<double> s1(1, {1, 2, 3, 4});
 * GenSerie<double> s2(3, {1,2,3, 4,5,6, 7,8,9, 10,11,12});
 *
 * auto parts = split(2, s1, s2); // vector<vector<GenSerie<double>>>
 * // parts[0][0]: {1,2}
 * // parts[0][1]: {3,4}
 * // parts[1][0]: {1,2,3, 4,5,6}
 * // parts[1][1]: {7,8,9, 10,11,12}
 * ```
 * @ingroup Utils
 */
template <typename T, typename... Series>
auto split(uint32_t n, const Series &...series) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type GenSerie<T>");

    if (n == 0) {
        throw std::invalid_argument("n must be > 0");
    }

    // Check that all series have the same count
    std::vector<uint32_t> counts = {series.count()...};
    if (!std::equal(counts.begin() + 1, counts.end(), counts.begin())) {
        throw std::invalid_argument("All series must have the same count");
    }

    auto split_one = [n](const GenSerie<T> &s) { return split(n, s); };

    std::vector<std::vector<GenSerie<T>>> result;
    (result.push_back(split_one(series)), ...);
    return result;
}

} // namespace utils
} // namespace df