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
#include <iostream>
#include <string>
#include <vector>

template <typename T> using ArrayType = std::vector<T>;

using String = std::string;
using Strings = std::vector<String>;

using iVector2 = std::array<uint, 2>;
using iVector3 = std::array<uint, 3>;
using iVector4 = std::array<uint, 4>;
using iVector6 = std::array<uint, 6>;
template <size_t N> using iVector = std::array<uint, N>;

using Vector2 = std::array<double, 2>;
using Vector3 = std::array<double, 3>;
using Vector4 = std::array<double, 4>;
using Vector6 = std::array<double, 6>;
template <size_t N> using Vector = std::array<double, N>;

using Matrix2D = std::array<double, 4>;  // Full
using Matrix3D = std::array<double, 9>;  // Full
using Matrix4D = std::array<double, 16>; // Full

using SMatrix2D = std::array<double, 3>;  // Symmetric
using SMatrix3D = std::array<double, 6>;  // Symmetric
using SMatrix4D = std::array<double, 10>; // Symmetric

using Strain2D = SMatrix2D;
using Strain3D = SMatrix3D;
using Stress2D = SMatrix2D;
using Stress3D = SMatrix3D;

namespace detail {

    /**
     * @brief Traits class for dimensionality of points or vectors (2D or 3D)
     */
    template <size_t DIM> struct point_type {};
    
    template <> struct point_type<2> {
        using type = Vector2;
    };
    
    template <> struct point_type<3> {
        using type = Vector3;
    };
    
    } // namespace detail
    

// ----------------------------------------------------------

template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &o, const std::array<T, N> &m) {
    o << "[";
    for (size_t i = 0; i < N - 1; ++i) {
        o << m[i] << ",";
    }
    o << m[m.size() - 1] << "]";

    return o;
}

