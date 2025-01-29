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
namespace math {

template <typename T>
details::IsSerieFloating<T> add(const Serie<T> &s1, const Serie<T> &s2) {
    if (s1.count() != s2.count()) {
        throw std::invalid_argument("Series must have same count");
    }

    auto result = Serie<T>(s1.itemSize(), s1.count());

    for (uint32_t i = 0; i < s1.count(); ++i) {
        if (s1.itemSize() > 1) {
            auto arr1 = s1.array(i);
            auto arr2 = s2.array(i);
            std::vector<T> diff(arr1.size());
            for (size_t j = 0; j < arr1.size(); ++j) {
                diff[j] = arr1[j] + arr2[j];
            }
            result.setArray(i, diff);
        } else {
            result.setValue(i, s1.value(i) + s2.value(i));
        }
    }

    return result;
}

} // namespace math
} // namespace df

template <typename T>
df::details::IsSerieFloating<T> operator+(const df::Serie<T> &s1,
                                          const df::Serie<T> &s2) {
    return df::math::add(s1, s2);
}
