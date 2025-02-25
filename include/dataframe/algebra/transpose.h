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
#include <array>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <stdexcept>

namespace df {

namespace detail {

// Helper for matrix transpose
template <typename T, size_t N>
std::array<T, N> transpose_matrix(const std::array<T, N> &m) {
    if constexpr (N == 4) { // 2x2 matrix
        return {m[0], m[2], m[1], m[3]};
    } else if constexpr (N == 9) { // 3x3 matrix
        return {m[0], m[3], m[6], m[1], m[4], m[7], m[2], m[5], m[8]};
    } else if constexpr (N == 16) { // 4x4 matrix
        return {m[0], m[4], m[8],  m[12], m[1], m[5], m[9],  m[13],
                m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]};
    } else {
        static_assert(N == 4 || N == 9 || N == 16,
                      "Transpose only supported for 2x2, 3x3 and 4x4 matrices");
        return m;
    }
}

} // namespace detail

/**
 * Compute matrix transpose
 * @param serie Input matrix serie
 * @return Serie containing transposed matrices
 */
template <typename T, size_t N>
Serie<std::array<T, N>> transpose(const Serie<std::array<T, N>> &serie) {
    static_assert(N == 4 || N == 9 || N == 16,
                  "Transpose only supported for 2x2, 3x3 and 4x4 matrices");

    return serie.map(
        [](const auto &m, size_t) { return detail::transpose_matrix(m); });
}

template <typename T, size_t N> auto bind_transpose() {
    return
        [](const Serie<std::array<T, N>> &serie) { return transpose(serie); };
}

} // namespace df