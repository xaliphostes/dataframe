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

// Single Serie print implementation
inline void print(const Serie &serie) {
    std::cout << "Serie:" << std::endl
              << "  itemSize : " << serie.itemSize() << std::endl
              << "  count    : " << serie.count() << std::endl
              << "  dimension: " << serie.dimension() << std::endl
              << "  values   : [";

    if (serie.count() > 0) {
        for (uint32_t i = 0; i < serie.count() - 1; ++i) {
            if (serie.itemSize() == 1) {
                std::cout << serie.get<double>(i) << ", ";
            } else {
                const auto &v = serie.get<Array>(i);
                std::cout << "(";
                for (size_t j = 0; j < v.size() - 1; ++j) {
                    std::cout << v[j] << ",";
                }
                std::cout << v.back() << "), ";
            }
        }
        // Last item
        if (serie.itemSize() == 1) {
            std::cout << serie.get<double>(serie.count() - 1);
        } else {
            const auto &v = serie.get<Array>(serie.count() - 1);
            std::cout << "(";
            for (size_t j = 0; j < v.size() - 1; ++j) {
                std::cout << v[j] << ",";
            }
            std::cout << v.back() << ")";
        }
    }
    std::cout << "]" << std::endl;
}

// Multiple Series print implementation
template <typename... Series>
void print(const Serie &first, const Series &...rest) {
    static_assert(std::conjunction<df::details::is_serie<Series>...>::value,
                  "All arguments must be Series");
    std::cout << "Series 1:" << std::endl;
    print(first);

    if constexpr (sizeof...(rest) > 0) {
        size_t index = 2;
        ((std::cout << "Series " << index++ << ":" << std::endl, print(rest)),
         ...);
    }
}

} // namespace utils
} // namespace df
