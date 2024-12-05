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

namespace df {

Serie sub(const std::initializer_list<Serie> &list);

Serie sub(const Serie &serie, double v);

inline Serie sub(const Serie& s1, const Serie& s2) {
    if (s1.itemSize() != s2.itemSize() || s1.count() != s2.count()) {
        throw std::invalid_argument("Series must have same itemSize and count");
    }

    Serie result(s1.itemSize(), s1.count());
    
    for (uint32_t i = 0; i < s1.count(); ++i) {
        if (s1.itemSize() == 1) {
            result.set(i, s1.get<double>(i) - s2.get<double>(i));
        } else {
            Array sum(s1.itemSize());
            auto v1 = s1.get<Array>(i);
            auto v2 = s2.get<Array>(i);
            for (size_t j = 0; j < s1.itemSize(); ++j) {
                sum[j] = v1[j] - v2[j];
            }
            result.set(i, sum);
        }
    }
    return result;
}

template <typename... Series> Serie add(const Series &...series) {
    if constexpr (sizeof...(series) < 1) {
        throw std::invalid_argument(
            "Number of arguments (Serie) must be greater than 1. Got " +
            std::to_string(sizeof...(series)) + " arguments)");
    }

    utils::countAndCheck(series...);

    auto first = std::get<0>(std::tuple<const Series &...>(series...));

    Serie result = Serie(first.itemSize(), first.count());
    ((result = sub(result, series)), ...);
    return result;
}

MAKE_OP(sub);

} // namespace df
