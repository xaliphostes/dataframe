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
#include <tuple>

namespace df {

/**
 * @brief Unzips a Serie into multiple Series based on provided itemSizes
 * @param zipped The zipped Serie
 * @param itemSizes Vector containing the itemSize for each output Serie
 * @return Series The unzipped Series
 */
Series unzip(const Serie &zipped, const std::vector<uint32_t> &itemSizes) {
    if (itemSizes.empty()) {
        throw std::invalid_argument("itemSizes cannot be empty");
    }

    uint32_t totalItemSize =
        std::accumulate(itemSizes.begin(), itemSizes.end(), 0u);
    if (totalItemSize != zipped.itemSize()) {
        throw std::invalid_argument("Total itemSize mismatch");
    }

    std::vector<Serie> results;
    results.reserve(itemSizes.size());

    uint32_t currentIndex = 0;
    for (uint32_t itemSize : itemSizes) {
        Serie s(itemSize, zipped.count());
        for (uint32_t i = 0; i < zipped.count(); ++i) {
            if (itemSize == 1) {
                s.setScalar(
                    i, zipped.scalar(i * zipped.itemSize() + currentIndex));
            } else {
                Array values(itemSize);
                for (uint32_t j = 0; j < itemSize; ++j) {
                    values[j] =
                        zipped.scalar(i * zipped.itemSize() + currentIndex + j);
                }
                s.setValue(i, values);
            }
        }
        results.push_back(s);
        currentIndex += itemSize;
    }

    return results;
}

} // namespace df