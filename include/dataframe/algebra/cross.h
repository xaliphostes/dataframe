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
// Helper for cross product (3D vectors only)
template <typename T>
std::array<T, 3> cross_3d(const std::array<T, 3> &a,
                          const std::array<T, 3> &b) {
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]};
}

} // namespace detail

/**
 * Compute cross product of two 3D vectors
 * @param serie1 First vector serie
 * @param serie2 Second vector serie
 * @return Serie containing cross products
 */
template <typename T>
Serie<std::array<T, 3>> cross(const Serie<std::array<T, 3>> &serie1,
                              const Serie<std::array<T, 3>> &serie2) {
    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same size for cross product");
    }

    return serie1.map([&serie2](const auto &a, size_t i) {
        return detail::cross_3d(a, serie2[i]);
    });
}

// Bind functions for pipeline operations
template <typename T> auto bind_cross(const Serie<std::array<T, 3>> &serie2) {
    return [serie2](const Serie<std::array<T, 3>> &serie1) {
        return cross(serie1, serie2);
    };
}

} // namespace df