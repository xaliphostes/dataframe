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
#include <dataframe/utils.h>
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

// 1x1 matrix inversion (scalar)
template <typename T> std::array<T, 1> inv_1x1(const std::array<T, 1> &m) {
    if (std::abs(m[0]) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    return {T{1} / m[0]};
}

// 2x2 matrix inversion (non-symmetric)
template <typename T> std::array<T, 4> inv_2x2(const std::array<T, 4> &m) {
    // m = [a b; c d] stored as [a,b,c,d]
    T det = m[0] * m[3] - m[1] * m[2];
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    T inv_det = T{1} / det;
    return {
        m[3] * inv_det,  // d/det
        -m[1] * inv_det, // -b/det
        -m[2] * inv_det, // -c/det
        m[0] * inv_det   // a/det
    };
}

// 2x2 symmetric matrix inversion
template <typename T> std::array<T, 3> inv_sym_2x2(const std::array<T, 3> &m) {
    // m = [a b; b c] stored as [a,b,c]
    T det = m[0] * m[2] - m[1] * m[1];
    if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Matrix is singular");
    }
    T inv_det = T{1} / det;
    return {
        m[2] * inv_det,  // c/det
        -m[1] * inv_det, // -b/det
        m[0] * inv_det   // a/det
    };
}

// 3x3 matrix inversion (non-symmetric)
template <typename T> std::array<T, 9> inv_3x3(const std::array<T, 9> &m) {
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
template <typename T> std::array<T, 6> inv_sym_3x3(const std::array<T, 6> &m) {
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

// Generic template for larger matrices using LU decomposition
template <typename T, size_t N>
std::array<T, N * N> inv_NxN(const std::array<T, N * N> &m) {
    // This is a placeholder for N>3 matrices
    // Implement LU decomposition or other suitable method here
    throw std::runtime_error(
        "Matrix inversion not implemented for this dimension");
}

} // namespace detail

/**
 * Compute the inverse of a matrix stored as an array
 * Supports 1x1, 2x2, 3x3 matrices (both symmetric and non-symmetric storage)
 *
 * @param serie Input Serie containing matrices
 * @return Serie containing inverted matrices
 * @throws std::runtime_error if matrix is singular or dimension not supported
 */
template <typename T, size_t N>
Serie<std::array<T, N>> inverse(const Serie<std::array<T, N>> &serie) {
    static_assert(std::is_floating_point<T>::value,
                  "Matrix inversion requires floating point type");

    return serie.map([](const std::array<T, N> &m, size_t) {
        if constexpr (N == 1) {
            return detail::inv_1x1(m);
        } else if constexpr (N == 3) {
            return detail::inv_sym_2x2(m);
        } else if constexpr (N == 4) {
            return detail::inv_2x2(m);
        } else if constexpr (N == 6) {
            return detail::inv_sym_3x3(m);
        } else if constexpr (N == 9) {
            return detail::inv_3x3(m);
        } else {
            static_assert(N <= 9, "Matrix dimension not supported");
            return detail::inv_NxN(m);
        }
    });
}

// MAKE_OP(inv)

// Shorter alias using bind_inv
template <typename T, size_t N> auto bind_inv() {
    return [](const Serie<std::array<T, N>> &serie) { return inverse(serie); };
}

// Alternative version that deduces types from input
template <typename T, size_t N>
auto bind_inv(const Serie<std::array<T, N>> &serie) {
    return inverse(serie);
}

} // namespace df