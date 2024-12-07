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
#include <dataframe/utils.h>

namespace df {
namespace utils {

/**
 * @brief Partition a Serie into n sub-series of equal size
 * @example
 * ```cpp
 *  Serie s1(1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
 *
 *  // Partition single Serie into 3 parts
 *  auto parts = partition_n(3, s1); // vector<Serie>
 *  // parts[0]: {1,2,3,4}
 *  // parts[1]: {5,6,7}
 *  // parts[2]: {8,9,10}
 * ```
 */
Series partition_n(uint32_t n, const Serie &serie) {
    uint32_t total = serie.count();
    uint32_t base_size = total / n;
    uint32_t remainder = total % n;

    std::vector<Serie> partitions;
    partitions.reserve(n);

    uint32_t start = 0;
    for (uint32_t i = 0; i < n; ++i) {
        uint32_t size = base_size + (i < remainder ? 1 : 0);
        Serie part(serie.itemSize(), size);
        for (uint32_t j = 0; j < size; ++j) {
            part.set(j, serie.get<Array>(start + j));
        }
        start += size;
        partitions.push_back(part);
        if (start >= total) {
            break;
        }
    }

    return partitions;
}

/**
 * @brief Partition n Series into n sub-series of equal size
 * @example
 * ```cpp
 * Serie s1(1, {1, 2, 3, 4});
 * Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
 *
 * // Partition 2 Series into 2 parts
 * auto parts = partition_n(2, s1, s2); // vector<Series>
 * // parts[0][0]: {1,2}
 * // parts[0][1]: {3,4}
 * // parts[1][0]: {1, 2, 3, 4, 5, 6}
 * // parts[1][1]: {7, 8, 9, 10, 11, 12}
 * ```
 */
template <typename... Series>
auto partition_n(uint32_t n, const Series &...series) {
    if (n == 0) {
        throw std::invalid_argument("n must be > 0");
    }

    auto counts = utils::countAndCheck(series...);
    uint32_t total = counts[0];
    uint32_t base_size = total / n;
    uint32_t remainder = total % n;

    auto partition_one = [n, base_size, remainder](const Serie &s) {
        std::vector<Serie> partitions;
        partitions.reserve(n);

        uint32_t start = 0;
        for (uint32_t i = 0; i < n; ++i) {
            uint32_t size = base_size + (i < remainder ? 1 : 0);
            Serie part(s.itemSize(), size);
            for (uint32_t j = 0; j < size; ++j) {
                part.set(j, s.get<Array>(start + j));
            }
            start += size;
            partitions.push_back(part);
        }
        return partitions;
    };

    std::vector<std::vector<Serie>> result;
    (result.push_back(partition_one(series)), ...);
    return result;
}

} // namespace utils
} // namespace df
