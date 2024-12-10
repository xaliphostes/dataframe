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
 * Concatenate 2 or more series
 */
Serie concat(const std::vector<Serie> &series) {
    if (series.empty()) {
        throw std::invalid_argument("concat requires at least one Serie");
    }

    uint32_t itemSize = series[0].itemSize();
    uint32_t totalCount = 0;

    auto counts = utils::countAndCheck();
    for (const auto &s : series) {
        if (!s.isValid() || s.itemSize() != itemSize) {
            throw std::invalid_argument(
                "All series must be valid and have same itemSize");
        }
        totalCount += s.count();
    }

    Serie result(itemSize, totalCount);
    uint32_t currentIndex = 0;

    for (const auto &s : series) {
        for (uint32_t i = 0; i < s.count(); ++i) {
            result.set(currentIndex++, s.get<Array>(i));
        }
    }

    return result;
}

template <typename... Series> Serie concat(const Series &...series) {
    std::vector<Serie> vec{series...};
    return concat(vec);
}

MAKE_OP(concat);

} // namespace utils
} // namespace df