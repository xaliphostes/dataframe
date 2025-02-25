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
 */

#pragma once
#include <array>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <stdexcept>

namespace df {

namespace detail {

// Helper to determine if we're dealing with a symmetric matrix storage
template <typename T, size_t N>
struct is_symmetric_storage : std::false_type {};

template <typename T>
struct is_symmetric_storage<T, 1> : std::true_type {}; // 1D -> symmetric 1x1

template <typename T>
struct is_symmetric_storage<T, 3> : std::true_type {}; // 3D -> symmetric 2x2

template <typename T>
struct is_symmetric_storage<T, 6> : std::true_type {}; // 6D -> symmetric 3x3

template <typename T>
struct is_symmetric_storage<T, 10> : std::true_type {}; // 10D -> symmetric 4x4

// 1x1 matrix inversion (scalar)
template <typename T> std::array<T, 1> inv_1x1(const std::array<T, 1> &m) {
    if (std::abs(m[0]) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    return {T{1} / m[0]};
}

// 2x2 matrix inversion (non-symmetric)
template <typename T> Matrix2D inv_2x2(const Matrix2D &m) {
    // m = [a b; c d] stored as [a,b,c,d]
    T det = m[0] * m[3] - m[1] * m[2];
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    T inv_det = T{1} / det;
    return {
        m[3] * inv_det,  //  d/det
        -m[1] * inv_det, // -b/det
        -m[2] * inv_det, // -c/det
        m[0] * inv_det   //  a/det
    };
}

// 2x2 symmetric matrix inversion
template <typename T> SMatrix2D inv_sym_2x2(const SMatrix2D &m) {
    // m = [a b; b c] stored as [a,b,c]
    T det = m[0] * m[2] - m[1] * m[1];
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    T inv_det = T{1} / det;
    return {
        m[2] * inv_det,  //  c/det
        -m[1] * inv_det, // -b/det
        m[0] * inv_det   //  a/det
    };
}

// 3x3 matrix inversion (non-symmetric)
template <typename T> Matrix3D inv_3x3(const Matrix3D &m) {
    // m = [a b c; d e f; g h i] stored as [a,b,c,d,e,f,g,h,i]
    T det = m[0] * m[4] * m[8] + m[1] * m[5] * m[6] + m[2] * m[3] * m[7] -
            m[2] * m[4] * m[6] - m[1] * m[3] * m[8] - m[0] * m[5] * m[7];

    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }

    T inv_det = T{1} / det;
    return {(m[4] * m[8] - m[5] * m[7]) * inv_det,
            (m[2] * m[7] - m[1] * m[8]) * inv_det,
            (m[1] * m[5] - m[2] * m[4]) * inv_det,
            (m[5] * m[6] - m[3] * m[8]) * inv_det,
            (m[0] * m[8] - m[2] * m[6]) * inv_det,
            (m[2] * m[3] - m[0] * m[5]) * inv_det,
            (m[3] * m[7] - m[4] * m[6]) * inv_det,
            (m[1] * m[6] - m[0] * m[7]) * inv_det,
            (m[0] * m[4] - m[1] * m[3]) * inv_det};
}

// 3x3 symmetric matrix inversion
template <typename T> SMatrix3D inv_sym_3x3(const SMatrix3D &m) {
    // m = [a b c; b d e; c e f] stored as [a,b,c,d,e,f]
    T det = m[0] * (m[3] * m[5] - m[4] * m[4]) -
            m[1] * (m[1] * m[5] - m[4] * m[2]) +
            m[2] * (m[1] * m[4] - m[3] * m[2]);

    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }

    T inv_det = T{1} / det;
    return {(m[3] * m[5] - m[4] * m[4]) * inv_det,
            (m[4] * m[2] - m[1] * m[5]) * inv_det,
            (m[1] * m[4] - m[3] * m[2]) * inv_det,
            (m[0] * m[5] - m[2] * m[2]) * inv_det,
            (m[2] * m[1] - m[0] * m[4]) * inv_det,
            (m[0] * m[3] - m[1] * m[1]) * inv_det};
}

// Helper function to get 3x3 determinant for 4x4 matrix cofactors
template <typename T>
T det3x3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22) {
    return m00 * (m11 * m22 - m12 * m21) - m01 * (m10 * m22 - m12 * m20) +
           m02 * (m10 * m21 - m11 * m20);
}

// 4x4 symmetric matrix inversion
template <typename T>
std::array<T, 10> inv_sym_4x4(const std::array<T, 10> &m) {
    // m = [a b c d; b e f g; c f h i; d g i j] stored as [a,b,c,d,e,f,g,h,i,j]

    // Calculate determinant using cofactor expansion along first row
    T det = m[0] * det3x3(m[4], m[5], m[6],   // [e f g]
                          m[5], m[7], m[8],   // [f h i]
                          m[6], m[8], m[9]) - // [g i j]
            m[1] * det3x3(m[1], m[5], m[6],   // [b f g]
                          m[2], m[7], m[8],   // [c h i]
                          m[3], m[8], m[9]) + // [d i j]
            m[2] * det3x3(m[1], m[2], m[6],   // [b c g]
                          m[2], m[5], m[8],   // [c f i]
                          m[3], m[6], m[9]) - // [d g j]
            m[3] * det3x3(m[1], m[2], m[3],   // [b c d]
                          m[2], m[5], m[6],   // [c f g]
                          m[3], m[6], m[8]);  // [d g i]

    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }

    T inv_det = T{1} / det;

    // Calculate cofactors and build inverse (taking advantage of symmetry)
    std::array<T, 10> result;

    // First row/column (a11, a12, a13, a14)
    result[0] = (m[4] * (m[7] * m[9] - m[8] * m[8]) +
                 m[5] * (m[8] * m[6] - m[5] * m[9]) +
                 m[6] * (m[5] * m[8] - m[7] * m[6])) *
                inv_det;

    result[1] = (m[1] * (m[8] * m[8] - m[7] * m[9]) +
                 m[2] * (m[6] * m[9] - m[8] * m[6]) +
                 m[3] * (m[7] * m[6] - m[5] * m[8])) *
                inv_det;

    result[2] = (m[1] * (m[5] * m[9] - m[6] * m[8]) +
                 m[2] * (m[6] * m[6] - m[4] * m[9]) +
                 m[3] * (m[4] * m[8] - m[5] * m[6])) *
                inv_det;

    result[3] = (m[1] * (m[6] * m[7] - m[5] * m[8]) +
                 m[2] * (m[4] * m[8] - m[6] * m[5]) +
                 m[3] * (m[5] * m[5] - m[4] * m[7])) *
                inv_det;

    // Second row/column (a22, a23, a24)
    result[4] = (m[0] * (m[7] * m[9] - m[8] * m[8]) +
                 m[2] * (m[3] * m[8] - m[2] * m[9]) +
                 m[3] * (m[2] * m[8] - m[3] * m[7])) *
                inv_det;

    result[5] = (m[0] * (m[5] * m[9] - m[6] * m[8]) +
                 m[1] * (m[6] * m[3] - m[2] * m[9]) +
                 m[3] * (m[2] * m[6] - m[3] * m[5])) *
                inv_det;

    result[6] = (m[0] * (m[6] * m[7] - m[5] * m[8]) +
                 m[1] * (m[2] * m[8] - m[3] * m[6]) +
                 m[2] * (m[3] * m[5] - m[2] * m[7])) *
                inv_det;

    // Third row/column (a33, a34)
    result[7] = (m[0] * (m[4] * m[9] - m[6] * m[6]) +
                 m[1] * (m[6] * m[3] - m[1] * m[9]) +
                 m[3] * (m[1] * m[6] - m[3] * m[4])) *
                inv_det;

    result[8] = (m[0] * (m[6] * m[5] - m[4] * m[8]) +
                 m[1] * (m[1] * m[8] - m[2] * m[6]) +
                 m[2] * (m[2] * m[4] - m[1] * m[5])) *
                inv_det;

    // Fourth row/column (a44)
    result[9] = (m[0] * (m[4] * m[7] - m[5] * m[5]) +
                 m[1] * (m[5] * m[2] - m[1] * m[7]) +
                 m[2] * (m[1] * m[5] - m[2] * m[4])) *
                inv_det;

    return result;
}

// 4x4 matrix inversion
template <typename T> Matrix4D inv_4x4(const Matrix4D &m) {
    // Get matrix cofactors
    Matrix4D cofactors;

    // First row cofactors
    cofactors[0] =
        det3x3(m[5], m[6], m[7], m[9], m[10], m[11], m[13], m[14], m[15]);
    cofactors[1] =
        -det3x3(m[4], m[6], m[7], m[8], m[10], m[11], m[12], m[14], m[15]);
    cofactors[2] =
        det3x3(m[4], m[5], m[7], m[8], m[9], m[11], m[12], m[13], m[15]);
    cofactors[3] =
        -det3x3(m[4], m[5], m[6], m[8], m[9], m[10], m[12], m[13], m[14]);

    // Calculate determinant using first row expansion
    T det = m[0] * cofactors[0] + m[1] * cofactors[1] + m[2] * cofactors[2] +
            m[3] * cofactors[3];

    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }

    // Second row cofactors
    cofactors[4] =
        -det3x3(m[1], m[2], m[3], m[9], m[10], m[11], m[13], m[14], m[15]);
    cofactors[5] =
        det3x3(m[0], m[2], m[3], m[8], m[10], m[11], m[12], m[14], m[15]);
    cofactors[6] =
        -det3x3(m[0], m[1], m[3], m[8], m[9], m[11], m[12], m[13], m[15]);
    cofactors[7] =
        det3x3(m[0], m[1], m[2], m[8], m[9], m[10], m[12], m[13], m[14]);

    // Third row cofactors
    cofactors[8] =
        det3x3(m[1], m[2], m[3], m[5], m[6], m[7], m[13], m[14], m[15]);
    cofactors[9] =
        -det3x3(m[0], m[2], m[3], m[4], m[6], m[7], m[12], m[14], m[15]);
    cofactors[10] =
        det3x3(m[0], m[1], m[3], m[4], m[5], m[7], m[12], m[13], m[15]);
    cofactors[11] =
        -det3x3(m[0], m[1], m[2], m[4], m[5], m[6], m[12], m[13], m[14]);

    // Fourth row cofactors
    cofactors[12] =
        -det3x3(m[1], m[2], m[3], m[5], m[6], m[7], m[9], m[10], m[11]);
    cofactors[13] =
        det3x3(m[0], m[2], m[3], m[4], m[6], m[7], m[8], m[10], m[11]);
    cofactors[14] =
        -det3x3(m[0], m[1], m[3], m[4], m[5], m[7], m[8], m[9], m[11]);
    cofactors[15] =
        det3x3(m[0], m[1], m[2], m[4], m[5], m[6], m[8], m[9], m[10]);

    // Transpose and divide by determinant
    T inv_det = T{1} / det;
    Matrix4D result;
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            result[i * 4 + j] = cofactors[j * 4 + i] * inv_det;
        }
    }

    return result;
}

// Generic template for larger matrices using LU decomposition
template <typename T, size_t N>
std::array<T, N * N> inv_NxN(const std::array<T, N * N> &m) {
    // This is a placeholder for N>4 matrices
    // Implement LU decomposition or other suitable method here
    throw std::runtime_error(
        "Matrix inversion not implemented for this dimension");
}

} // namespace detail

// ------------------------------------------------------------------

/**
 * Compute the inverse of a matrix stored as an array
 * Supports 1x1, 2x2, 3x3, 4x4 matrices (both symmetric and non-symmetric
 * storage)
 *
 * @param serie Input Serie containing matrices
 * @return Serie containing inverted matrices
 * @throws std::runtime_error if matrix is singular or dimension not supported
 */
template <typename T, size_t N>
inline Serie<std::array<T, N>> inv(const Serie<std::array<T, N>> &serie) {
    static_assert(std::is_floating_point<T>::value,
                  "Matrix inversion requires floating point type");

    return serie.map([](const std::array<T, N> &m, size_t) {
        if constexpr (N == 1) {
            return detail::inv_1x1(m);
        } else if constexpr (N == 3) {
            return detail::inv_sym_2x2<T>(m);
        } else if constexpr (N == 4) {
            return detail::inv_2x2<T>(m);
        } else if constexpr (N == 6) {
            return detail::inv_sym_3x3<T>(m);
        } else if constexpr (N == 9) {
            return detail::inv_3x3<T>(m);
        } else if constexpr (N == 10) {
            return detail::inv_sym_4x4<T>(m);
        } else if constexpr (N == 16) {
            return detail::inv_4x4<T>(m);
        } else {
            static_assert(N <= 16, "Matrix dimension not supported");
            return detail::inv_NxN(m);
        }
    });
}

// MAKE_OP(inv)

// Shorter alias using bind_inv
template <typename T, size_t N> inline auto bind_inv() {
    return [](const Serie<std::array<T, N>> &serie) { return inv(serie); };
}

// Alternative version that deduces types from input
template <typename T, size_t N>
inline auto bind_inv(const Serie<std::array<T, N>> &serie) {
    return inverse(serie);
}

} // namespace df