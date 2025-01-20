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
#include <tuple>

namespace df {
namespace utils {

/**
 * @brief Zips multiple GenSeries together into a single GenSerie
 */
template <typename T, typename... Series>
GenSerie<T> zip(const GenSerie<T> &first, const Series &...rest) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type GenSerie<T>");

    // Check if all series have the same count
    const uint32_t count = first.count();
    if (!((rest.count() == count) && ...)) {
        throw std::invalid_argument("All series must have the same count");
    }

    // Calculate total itemSize
    const uint32_t totalItemSize = (first.itemSize() + ... + rest.itemSize());

    // Create result serie
    GenSerie<T> result(totalItemSize, count);

    // Helper to copy values from input series to result
    auto copyValues = [&result](uint32_t itemIndex, const GenSerie<T> &s) {
        for (uint32_t i = 0; i < s.count(); ++i) {
            if (s.itemSize() == 1) {
                result.setValue(i * result.itemSize() + itemIndex, s.value(i));
            } else {
                auto values = s.array(i);
                for (uint32_t j = 0; j < s.itemSize(); ++j) {
                    result.setValue(i * result.itemSize() + itemIndex + j,
                                     values[j]);
                }
            }
        }
        return itemIndex + s.itemSize();
    };

    // Copy all values from input series
    uint32_t currentIndex = copyValues(0, first);
    (..., (currentIndex = copyValues(currentIndex, rest)));

    return result;
}

/**
 * @brief Zips multiple GenSeries together into a single GenSerie
 * @example
 * ```cpp
 * GenSerie<double> s1(1, {1, 2, 3});       // scalar serie
 * GenSerie<double> s2(2, {4,5, 6,7, 8,9}); // 2D serie
 * GenSerie<double> s3(1, {10, 11, 12});    // scalar serie
 *
 * // Results in a GenSerie with itemSize 4 (1+2+1)
 * auto result = zip(s1, s2, s3);
 * ```
 */
template <typename T, typename... Series>
GenSerie<T> zip(const Series &...series) {
    static_assert((std::is_same_v<Series, GenSerie<T>> && ...),
                  "All series must be of the same type GenSerie<T>");
    static_assert(sizeof...(series) > 0, "zip requires at least one serie");

    // Check if all series have the same count
    std::vector<uint32_t> counts = {series.count()...};
    if (!std::equal(counts.begin() + 1, counts.end(), counts.begin())) {
        throw std::invalid_argument("All series must have the same count");
    }

    // Calculate total itemSize
    const uint32_t totalItemSize = (... + series.itemSize());
    const uint32_t count = counts[0];

    // Create result serie
    GenSerie<T> result(totalItemSize, count);

    // Helper to copy values from input series to result
    auto copyValues = [&result](uint32_t itemIndex, const GenSerie<T> &s) {
        for (uint32_t i = 0; i < s.count(); ++i) {
            if (s.itemSize() == 1) {
                result.setScalar(i * result.itemSize() + itemIndex, s.value(i));
            } else {
                auto values = s.array(i);
                for (uint32_t j = 0; j < s.itemSize(); ++j) {
                    result.setScalar(i * result.itemSize() + itemIndex + j,
                                     values[j]);
                }
            }
        }
        return itemIndex + s.itemSize();
    };

    // Copy all values from input series
    uint32_t currentIndex = 0;
    (..., (currentIndex = copyValues(currentIndex, series)));

    return result;
}

/**
 * @brief Zips a vector of GenSeries together
 */
template <typename T> GenSerie<T> zip(const std::vector<GenSerie<T>> &series) {
    if (series.empty()) {
        throw std::invalid_argument("zip requires at least one serie");
    }

    // Check if all series have the same count
    const uint32_t count = series[0].count();
    for (const auto &s : series) {
        if (s.count() != count) {
            throw std::invalid_argument("All series must have the same count");
        }
    }

    // Calculate total itemSize
    uint32_t totalItemSize = 0;
    for (const auto &s : series) {
        totalItemSize += s.itemSize();
    }

    // Create result serie
    GenSerie<T> result(totalItemSize, count);

    // Copy values from all input series
    uint32_t currentIndex = 0;
    for (const auto &s : series) {
        for (uint32_t i = 0; i < count; ++i) {
            if (s.itemSize() == 1) {
                result.setValue(i * result.itemSize() + currentIndex,
                                 s.value(i));
            } else {
                auto values = s.array(i);
                for (uint32_t j = 0; j < s.itemSize(); ++j) {
                    result.setValue(i * result.itemSize() + currentIndex + j,
                                     values[j]);
                }
            }
        }
        currentIndex += s.itemSize();
    }

    return result;
}

} // namespace utils
} // namespace df