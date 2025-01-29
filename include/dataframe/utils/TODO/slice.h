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

namespace df {
namespace utils {

/**
 * @brief Takes a slice of a Serie from start to end indices
 * @example
 * ```cpp
 * Serie<double> s1(1, {1, 2, 3, 4, 5, 6});
 * auto result = slice(1, 4, s1); // Elements at indices 1,2,3
 * // result: {2, 3, 4}
 *
 * // Using with pipe operator
 * auto result2 = s1 | make_slice(1, 4);
 * ```
 * @param start Starting index (inclusive)
 * @param end Ending index (exclusive)
 * @ingroup Utils
 */
template <typename T>
Serie<T> slice(uint32_t start, uint32_t end, const Serie<T> &serie) {
    if (start >= serie.count()) {
        throw std::out_of_range("Start index out of range");
    }
    if (end > serie.count()) {
        throw std::out_of_range("End index out of range");
    }
    if (start >= end) {
        throw std::invalid_argument("Start index must be less than end index");
    }

    uint32_t size = end - start;
    Serie<T> result(serie.itemSize(), size);

    for (uint32_t i = 0; i < size; ++i) {
        if (serie.itemSize() == 1) {
            result.setValue(i, serie.value(start + i));
        } else {
            result.setArray(i, serie.array(start + i));
        }
    }

    return result;
}

/**
 * @brief Creates a slice operation that can be used with the pipe operator
 */
inline auto make_slice(uint32_t start, uint32_t end) {
    return [=](const auto &serie) {
        using T = typename std::decay_t<decltype(serie)>::value_type;
        return slice<T>(start, end, serie);
    };
}

} // namespace utils
} // namespace df