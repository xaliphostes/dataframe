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
// #include <dataframe/utils.h>
#include <stdexcept>

namespace df {

    template <size_t N> struct eigen_values_return_type;
    template <> struct eigen_values_return_type<3> {
        using type = std::array<double, 2>;
    };
    template <> struct eigen_values_return_type<6> {
        using type = std::array<double, 3>;
    };
    template <> struct eigen_values_return_type<10> {
        using type = std::array<double, 4>;
    };

    template <size_t N> struct eigen_vectors_return_type;
    template <> struct eigen_vectors_return_type<3> {
        using type = std::array<Vector2, 2>;
    };
    template <> struct eigen_vectors_return_type<6> {
        using type = std::array<Vector3, 3>;
    };
    template <> struct eigen_vectors_return_type<10> {
        using type = std::array<Vector4, 4>;
    };

    // ---------------------------------------------------------------

    /**
     * Compute eigenvalues of symmetric matrices
     * @param serie Input Serie containing symmetric matrices in row storage
     * format
     * @return Serie containing eigenvalues in descending order
     */
    template <typename T, size_t N>
    Serie<typename eigen_values_return_type<N>::type> eigenValues(
        const Serie<std::array<T, N>>& serie);

    /**
     * Compute eigenvectors of symmetric matrices
     * @param serie Input Serie containing symmetric matrices in row storage format
     * @return Serie containing eigenvectors in row storage format
     */
    template <typename T, size_t N>
    Serie<typename eigen_vectors_return_type<N>::type> eigenVectors(
        const Serie<std::array<T, N>>& serie);

    /**
     * Compute both eigenvectors and eigenvalues of symmetric matrices
     * @param serie Input Serie containing symmetric matrices in row storage format
     * @return std::pair of Series containing eigenvectors (row storage) and
     * eigenvalues
     */
    template <typename T, size_t N>
    std::pair<Serie<typename eigen_values_return_type<N>::type>,
        Serie<typename eigen_vectors_return_type<N>::type>>
    eigenSystem(const Serie<std::array<T, N>>& serie);

    /**
     * @brief Binding functions for pipeline operations
     */
    template <typename T, size_t N> auto bind_eigenVectors();
    template <typename T, size_t N> auto bind_eigenValues();
    template <typename T, size_t N> auto bind_eigenSystem();

} // namespace df

#include "inline/eigen.hxx"