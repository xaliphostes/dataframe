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
#include <dataframe/utils.h>
#include <stdexcept>

namespace df {

namespace detail {

// Helper for dot product
template <typename T, size_t N>
T dot_product(const std::array<T, N> &a, const std::array<T, N> &b) {
    T result = T{0};
    for (size_t i = 0; i < N; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

} // namespace detail

/**
 * Compute dot product of two vectors
 * @param serie1 First vector serie
 * @param serie2 Second vector serie
 * @return Serie containing dot products
 */
template <typename T, size_t N>
Serie<T> dot(const Serie<std::array<T, N>> &serie1,
             const Serie<std::array<T, N>> &serie2) {
    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same size for dot product");
    }

    return serie1.map([&serie2](const auto &a, size_t i) {
        return detail::dot_product(a, serie2[i]);
    });
}

template <typename T, size_t N> auto bind_dot(const Serie<std::array<T, N>> &serie2) {
    return [serie2](const Serie<std::array<T, N>> &serie1) {
        return dot(serie1, serie2);
    };
}

} // namespace df