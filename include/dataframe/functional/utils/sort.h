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
#include <algorithm>

namespace df {
namespace utils {

template <typename Comparator>
Serie sort(const Serie &serie, Comparator &&comp) {
    if (!serie.isValid())
        return Serie();

    if (serie.itemSize() == 1) {
        Array values;
        values.reserve(serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            values.push_back(serie.template get<double>(i));
        }

        std::sort(values.begin(), values.end());
        return Serie(1, values);
    } else {
        std::vector<Array> values;
        values.reserve(serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            values.push_back(serie.template get<Array>(i));
        }

        std::sort(values.begin(), values.end(), comp);

        Array flattened;
        flattened.reserve(serie.itemSize() * values.size());
        for (const auto &v : values) {
            flattened.insert(flattened.end(), v.begin(), v.end());
        }
        return Serie(serie.itemSize(), flattened);
    }
}

inline Serie sort(const Serie &serie) {
    if (serie.itemSize() == 1) {
        return sort(serie, std::less<>{});
    }
    return sort(serie, [](const Array &a, const Array &b) {
        for (size_t i = 0; i < a.size(); ++i) {
            if (a[i] != b[i])
                return a[i] < b[i];
        }
        return false;
    });
}

MAKE_OP(sort);

} // namespace utils
} // namespace df
