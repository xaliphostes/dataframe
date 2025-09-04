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
#include <cstddef>
#include <dataframe/Serie.h>
#include <dataframe/core/map.h>
#include <stdexcept>

namespace df {

    namespace detail {

        template <typename T, std::size_t N> struct Matrix {
            static_assert(N > 0, "N must be >= 1");
            std::array<T, N * N> d {};
            static constexpr std::size_t rows() noexcept;
            static constexpr std::size_t cols() noexcept;
            constexpr T& operator()(std::size_t i, std::size_t j) noexcept;
            constexpr const T& operator()(std::size_t i, std::size_t j) const noexcept;
            static Matrix Identity();
            void swap_columns(std::size_t j, std::size_t k);
        };

        constexpr std::size_t dim_from_packed_len(std::size_t P);
        constexpr std::size_t triangular(std::size_t m);

        template <typename T, size_t P> struct eigen_values_info {
            static constexpr std::size_t M = dim_from_packed_len(P);
            static_assert(triangular(M) == P, "Packed length must be triangular (1,3,6,10,...)");
            using type = std::array<T, M>;
        };

        template <typename T, size_t P> struct eigen_vectors_info {
            static constexpr std::size_t M = dim_from_packed_len(P);
            static_assert(triangular(M) == P, "Packed length must be triangular (1,3,6,10,...)");
            using type = std::array<std::array<T, M>, M>;
        };

    }

    template <typename T, std::size_t N> struct EigenResult {
        std::array<T, N> values; // eigenvalues
        detail::Matrix<T, N> vectors; // eigenvectors in COLUMNS (row-major storage)
        bool converged;
        std::size_t sweeps;
    };

    template <typename T, size_t N>
    Serie<typename detail::eigen_values_info<T, N>::type> eigenValues(
        const Serie<std::array<T, N>>& serie);

    template <typename T, size_t N>
    Serie<typename detail::eigen_vectors_info<T, N>::type> eigenVectors(
        const Serie<std::array<T, N>>& serie);

    template <typename T, size_t N>
    inline Serie<std::pair<typename detail::eigen_values_info<T, N>::type,
        typename detail::eigen_vectors_info<T, N>::type>>
    eigenSystem(const Serie<std::array<T, N>>& serie);

} // namespace df

#include "inline/eigen.hxx"
