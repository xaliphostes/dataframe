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
#include <dataframe/meta.h>

namespace df {
namespace algebra {

// Calculate cross product for 2D vectors (returns scalar)
template <typename T>
std::enable_if_t<details::is_floating_v<T>, T>
cross2D(const std::vector<T> &v1, const std::vector<T> &v2) {
    if (v1.size() < 2 || v2.size() < 2) {
        throw std::invalid_argument(
            "Vectors must have at least 2 components for 2D cross product");
    }
    return v1[0] * v2[1] - v1[1] * v2[0];
}

// Calculate cross product for 3D vectors (returns vector)
template <typename T>
std::enable_if_t<details::is_floating_v<T>, std::vector<T>>
cross3D(const std::vector<T> &v1, const std::vector<T> &v2) {
    if (v1.size() < 3 || v2.size() < 3) {
        throw std::invalid_argument(
            "Vectors must have at least 3 components for 3D cross product");
    }
    return {v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
            v1[0] * v2[1] - v1[1] * v2[0]};
}

// Cross product between two series
template <typename T>
std::enable_if_t<details::is_floating_v<T>, Serie<T>>
cross(const Serie<T> &s1, const Serie<T> &s2) {
    if (s1.count() != s2.count()) {
        throw std::invalid_argument("Series must have the same count");
    }
    if (s1.itemSize() != s2.itemSize()) {
        throw std::invalid_argument("Series must have the same itemSize");
    }

    const auto itemSize = s1.itemSize();
    if (itemSize < 2) {
        throw std::invalid_argument(
            "Series itemSize must be at least 2 for cross product");
    }

    // For 2D vectors (itemSize == 2), result is scalar (itemSize == 1)
    // For 3D vectors (itemSize >= 3), result has same itemSize as input
    const auto resultItemSize = (itemSize == 2) ? 1 : 3;
    Serie<T> result(resultItemSize, s1.count());

    for (uint32_t i = 0; i < s1.count(); ++i) {
        const auto v1 = s1.array(i);
        const auto v2 = s2.array(i);

        if (itemSize == 2) {
            // 2D cross product (returns scalar)
            result.setValue(i, cross2D(v1, v2));
        } else {
            // 3D cross product (returns vector)
            result.setArray(i, cross3D(v1, v2));
        }
    }

    return result;
}

// Cross product with constant vector
template <typename T>
std::enable_if_t<details::is_floating_v<T>, Serie<T>>
cross(const Serie<T> &serie, const std::vector<T> &constant) {
    const auto itemSize = serie.itemSize();
    if (itemSize != constant.size() || itemSize < 2) {
        throw std::invalid_argument("Serie itemSize and constant vector size "
                                    "must match and be at least 2");
    }

    const auto resultItemSize = (itemSize == 2) ? 1 : 3;
    Serie<T> result(resultItemSize, serie.count());

    for (uint32_t i = 0; i < serie.count(); ++i) {
        const auto v = serie.array(i);

        if (itemSize == 2) {
            result.setValue(i, cross2D(v, constant));
        } else {
            result.setArray(i, cross3D(v, constant));
        }
    }

    return result;
}

// Overload for constant vector on the left
template <typename T>
std::enable_if_t<details::is_floating_v<T>, Serie<T>>
cross(const std::vector<T> &constant, const Serie<T> &serie) {
    const auto itemSize = serie.itemSize();
    if (itemSize != constant.size() || itemSize < 2) {
        throw std::invalid_argument("Serie itemSize and constant vector size "
                                    "must match and be at least 2");
    }

    const auto resultItemSize = (itemSize == 2) ? 1 : 3;
    Serie<T> result(resultItemSize, serie.count());

    for (uint32_t i = 0; i < serie.count(); ++i) {
        const auto v = serie.array(i);

        if (itemSize == 2) {
            result.setValue(i, cross2D(constant, v));
        } else {
            result.setArray(i, cross3D(constant, v));
        }
    }

    return result;
}

} // namespace algebra
} // namespace df
