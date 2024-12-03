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
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace df {

/**
 * If concatenate is false, interleave is used to merge the series
 */
Serie merge(const Serie &s1, const Serie &s2, bool concatenate = true) {
    if (s1.itemSize() != s2.itemSize()) {
        throw std::invalid_argument("Series must have same itemSize");
    }

    if (concatenate) {
        Serie result(s1.itemSize(), s1.count() + s2.count());
        uint32_t idx = 0;

        for (uint32_t i = 0; i < s1.count(); ++i, ++idx) {
            result.set(idx, s1.template get<Array>(i));
        }
        for (uint32_t i = 0; i < s2.count(); ++i, ++idx) {
            result.set(idx, s2.template get<Array>(i));
        }
        return result;
    } else { // Interleave
        Serie result(s1.itemSize(), s1.count() + s2.count());
        uint32_t idx = 0;

        for (uint32_t i = 0; i < std::max(s1.count(), s2.count()); ++i) {
            if (i < s1.count()) {
                result.set(idx++, s1.template get<Array>(i));
            }
            if (i < s2.count()) {
                result.set(idx++, s2.template get<Array>(i));
            }
        }
        return result;
    }
}

// Variadic merge
template <typename... Series>
Serie merge(bool concatenate, const Series &...series) {
    std::array<size_t, sizeof...(series)> itemSizes = {series.itemSize()...};
    if (!std::equal(itemSizes.begin() + 1, itemSizes.end(),
                    itemSizes.begin())) {
        throw std::invalid_argument("All series must have same itemSize");
    }

    uint32_t total_count = (series.count() + ...);
    Serie result(std::get<0>(std::forward_as_tuple(series...)).itemSize(),
                 total_count);

    if (concatenate) {
        uint32_t idx = 0;
        (([&](const auto &s) {
             for (uint32_t i = 0; i < s.count(); ++i) {
                 result.set(idx++, s.template get<Array>(i));
             }
         }(series)),
         ...);
    } else { // Interleave
        uint32_t max_count = std::max({series.count()...});
        uint32_t idx = 0;
        for (uint32_t i = 0; i < max_count; ++i) {
            (([&](const auto &s) {
                 if (i < s.count()) {
                     result.set(idx++, s.template get<Array>(i));
                 }
             }(series)),
             ...);
        }
    }
    return result;
}

} // namespace df
