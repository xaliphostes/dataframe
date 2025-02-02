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

namespace df {

namespace detail {

// Helper to compute 2D determinant
template <typename T> T det2D(const std::array<T, 2> &vec) {
    return vec[0]; // For 2D vector, det is just the first component
}

template <typename T> T det2D(const std::array<T, 3> &mat) {
    // For 2x2 matrix stored as [a11, a12, a21], det = a11*a21 - a12*a21
    return mat[0] * mat[2] - mat[1] * mat[1];
}

// Helper to compute 3D determinant
template <typename T> T det3D(const std::array<T, 3> &vec) {
    return vec[0]; // For 3D vector, det is just the first component
}

template <typename T> T det3D(const std::array<T, 6> &mat) {
    // For 3x3 symmetric matrix stored as [a11, a12, a13, a22, a23, a33]
    return mat[0] * (mat[3] * mat[5] - mat[4] * mat[4]) -
           mat[1] * (mat[1] * mat[5] - mat[4] * mat[2]) +
           mat[2] * (mat[1] * mat[4] - mat[3] * mat[2]);
}

// Type trait to determine array dimension
template <typename T> struct array_dimension;

template <typename T, std::size_t N> struct array_dimension<std::array<T, N>> {
    static constexpr std::size_t dimension = N;
};

} // namespace detail

template <typename T, std::size_t N>
inline Serie<T> det(const Serie<std::array<T, N>> &serie) {
    using ArrayType = std::array<T, N>;

    // Determine if we're dealing with 2D or 3D
    constexpr bool is2D =
        (N == 2 || N == 3); // 2D vector or 2D symmetric matrix
    constexpr bool is3D =
        (N == 3 || N == 6); // 3D vector or 3D symmetric matrix

    static_assert(
        is2D || is3D,
        "Array size must be compatible with 2D or 3D determinant calculation");

    return serie.map([](const ArrayType &arr, size_t) {
        if constexpr (N == 2 || N == 3) {
            return detail::det2D(arr);
        } else if constexpr (N == 6) {
            return detail::det3D(arr);
        }
    });
}

template <typename T, std::size_t N> inline auto bind_det() {
    return [](const Serie<std::array<T, N>> &serie) { return det(serie); };
}

} // namespace df