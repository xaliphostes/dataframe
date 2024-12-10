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
#include "Serie.h"

namespace df {
namespace utils {

/**
 * Get the counts of all series. If throwError is true, throws an error if all
 * series don't have the same count.
 */
template <typename... Series> auto count(const Series &...series) {
    std::array<size_t, sizeof...(series)> counts = {series.count()...};
    return counts;
}

inline std::vector<size_t> getCount(const std::vector<Serie> &series) {
    std::vector<size_t> counts;
    for (size_t i = 0; i < series.size(); ++i) {
        counts.push_back(series[i].count());
    }
    return counts;
}

template <typename... Series> auto countAndCheck(const Series &...series) {
    auto counts = df::utils::count(series...);
    for (size_t i = 1; i < counts.size(); ++i) {
        if (counts[i] != counts[0]) {
            throw std::invalid_argument(
                "All Series must have the same count. First is " +
                std::to_string(counts[0]) + " and " + std::to_string(i) +
                "eme is " + std::to_string(counts[i]));
        }
    }
    return counts;
}

inline std::vector<size_t> countAndCheck(const std::vector<Serie> &series) {
    auto counts = getCount(series);
    for (size_t i = 1; i < counts.size(); ++i) {
        if (counts[i] != counts[0]) {
            throw std::invalid_argument(
                "All Series must have the same count. First is " +
                std::to_string(counts[0]) + " and " + std::to_string(i) +
                "eme is " + std::to_string(counts[i]));
        }
    }
    return counts;
}

} // namespace utils
} // namespace df