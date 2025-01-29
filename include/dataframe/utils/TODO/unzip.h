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
 * @brief Unzips a Serie into multiple Series based on provided itemSizes
 * @example
 * ```cpp
 * Serie<double> zipped = ...;  // itemSize = 4
 * auto parts = unzip(zipped, {1, 2, 1}); // Split into sizes 1, 2, and 1
 * ```
 */
template <typename T>
std::vector<Serie<T>> unzip(const Serie<T> &zipped,
                               const std::vector<uint32_t> &itemSizes) {
    if (itemSizes.empty()) {
        throw std::invalid_argument("itemSizes cannot be empty");
    }

    uint32_t totalItemSize =
        std::accumulate(itemSizes.begin(), itemSizes.end(), 0u);
    if (totalItemSize != zipped.itemSize()) {
        throw std::invalid_argument("Total itemSize mismatch");
    }

    std::vector<Serie<T>> results;
    results.reserve(itemSizes.size());

    uint32_t currentIndex = 0;
    for (uint32_t itemSize : itemSizes) {
        Serie<T> s(itemSize, zipped.count());

        for (uint32_t i = 0; i < zipped.count(); ++i) {
            if (itemSize == 1) {
                s.setValue(i,
                           zipped.value(i * zipped.itemSize() + currentIndex));
            } else {
                std::vector<T> values(itemSize);
                for (uint32_t j = 0; j < itemSize; ++j) {
                    values[j] =
                        zipped.value(i * zipped.itemSize() + currentIndex + j);
                }
                s.setArray(i, values);
            }
        }
        results.push_back(std::move(s));
        currentIndex += itemSize;
    }

    return results;
}

} // namespace utils
} // namespace df