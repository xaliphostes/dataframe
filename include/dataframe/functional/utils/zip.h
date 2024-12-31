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
#include <dataframe/utils.h>
#include <stdexcept>
#include <tuple>

namespace df {
namespace utils {

/**
 * @brief Zips multiple Series together into a single Serie
 * @param series Variable number of input Series
 * @return Serie A new Serie containing combined values from input Series
 * @throws std::invalid_argument if input Series have different counts or are
 * invalid
 * @ingroup Utils
 *
 * @example
 * ```cpp
 * Serie s1(1, {1, 2, 3});       // scalar serie
 * Serie s2(2, {4,5, 6,7, 8,9}); // 2D serie
 * Serie s3(1, {10, 11, 12});    // scalar serie
 *
 * // Results in a Serie with itemSize 4 (1+2+1) and values {1,4,5,10, 2,6,7,11,
 * 3,8,9,12} auto result = zip(s1, s2, s3);
 * ```
 */
template <typename... Series> Serie zip(const Series &...series) {
    static_assert(sizeof...(series) > 0, "zip requires at least one Serie");

    // Helper to check if all series are valid
    auto checkValid = [](const auto &...s) { return (... && s.isValid()); };

    if (!checkValid(series...)) {
        throw std::invalid_argument("All input Series must be valid");
    }

    auto count = utils::countAndCheck(series...)[0];

    // Calculate total itemSize
    const uint32_t totalItemSize = (... + series.itemSize());

    // Create result Serie
    Serie result(totalItemSize, count);

    // Helper to copy values from input Series to result
    auto copyValues = [&result](uint32_t itemIndex, const Serie &s) {
        for (uint32_t i = 0; i < s.count(); ++i) {
            if (s.itemSize() == 1) {
                result.setScalar(i * result.itemSize() + itemIndex,
                                 s.scalar(i));
            } else {
                Array values = s.value(i);
                for (uint32_t j = 0; j < s.itemSize(); ++j) {
                    result.setScalar(i * result.itemSize() + itemIndex + j,
                                     values[j]);
                }
            }
        }
        return itemIndex + s.itemSize();
    };

    // Copy all values from input Series
    uint32_t currentIndex = 0;
    (..., (currentIndex = copyValues(currentIndex, series)));

    return result;
}

MAKE_OP(zip);

/**
 * @brief Zips a vector of Series together into a single Serie
 * @param series Vector of input Series
 * @return Serie A new Serie containing combined values from input Series
 * @throws std::invalid_argument if input Series have different counts or are
 * invalid
 * @ingroup Utils
 *
 * @example
 * ```cpp
 * std::vector<Serie> series = {
 *     Serie(1, {1, 2, 3}),
 *     Serie(2, {4,5, 6,7, 8,9}),
 *     Serie(1, {10, 11, 12})
 * };
 * auto result = zipVector(series);
 * ```
 */
Serie zipVector(const std::vector<Serie> &series) {
    if (series.empty()) {
        throw std::invalid_argument("zipVector requires at least one Serie");
    }

    // Check if all series are valid
    for (const auto &s : series) {
        if (!s.isValid()) {
            throw std::invalid_argument("All input Series must be valid");
        }
    }

    size_t count = utils::countAndCheck(series)[0];

    // Calculate total itemSize
    uint32_t totalItemSize = 0;
    for (const auto &s : series) {
        totalItemSize += s.itemSize();
    }

    // Create result Serie
    Serie result(totalItemSize, count);

    // Copy values from all input Series
    uint32_t currentIndex = 0;
    for (const auto &s : series) {
        for (uint32_t i = 0; i < count; ++i) {
            if (s.itemSize() == 1) {
                result.setScalar(i * result.itemSize() + currentIndex,
                                 s.scalar(i));
            } else {
                Array values = s.value(i);
                for (uint32_t j = 0; j < s.itemSize(); ++j) {
                    result.setScalar(i * result.itemSize() + currentIndex + j,
                                     values[j]);
                }
            }
        }
        currentIndex += s.itemSize();
    }

    return result;
}

MAKE_OP(zipVector);

} // namespace utils
} // namespace df