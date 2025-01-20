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
#include <dataframe/functional/utils/concat.h>
#include <stdexcept>
#include <tuple>

namespace df {
namespace utils {

template <typename T>
GenSerie<T> concat(const std::vector<GenSerie<T>> &series) {
    if (series.empty()) {
        return GenSerie<T>();
    }

    // Check that all series have the same itemSize
    uint32_t itemSize = series[0].itemSize();
    for (const auto &s : series) {
        if (s.itemSize() != itemSize) {
            throw std::invalid_argument(
                "All series must have the same itemSize");
        }
    }

    // Calculate total count
    uint32_t totalCount = 0;
    for (const auto &s : series) {
        totalCount += s.count();
    }

    // Create result series
    GenSerie<T> result(itemSize, totalCount);

    // Copy data from all series
    uint32_t currentIndex = 0;
    for (const auto &s : series) {
        for (uint32_t i = 0; i < s.count(); ++i) {
            if (itemSize == 1) {
                result.setValue(currentIndex, s.value(i));
            } else {
                result.setArray(currentIndex, s.array(i));
            }
            currentIndex++;
        }
    }

    return result;
}

// Variadic template version
template <typename T, typename... Series>
GenSerie<T> concat(const Series &...series) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type GenSerie<T>");
    std::vector<GenSerie<T>> vec{series...};
    return concat(vec);
}

// make_concat for a vector of series
template <typename T> auto make_concat(const std::vector<GenSerie<T>> &others) {
    return [&others](const GenSerie<T> &first) -> GenSerie<T> {
        std::vector<GenSerie<T>> all;
        all.reserve(others.size() + 1);
        all.push_back(first);
        all.insert(all.end(), others.begin(), others.end());
        return concat(all);
    };
}

// make_concat for variadic series
template <typename T, typename... Series>
auto make_concat(const Series &...others) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type GenSerie<T>");
    return [&others...](const GenSerie<T> &first) -> GenSerie<T> {
        return concat<T>(first, others...);
    };
}

} // namespace utils
} // namespace df