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
#include "../Serie.h"
#include "types.h"

namespace df {

    /**
     * @brief Compute the determinant for each FullMatrix in the Serie
     *
     * This function applies the determinant operation to each FullMatrix element
     * in the input Serie, returning a Serie of scalar determinant values.
     *
     * @tparam T The element type (e.g., double, float)
     * @tparam N The matrix dimension (2 for 2x2, 3 for 3x3, etc.)
     * @param serie Input Serie containing FullMatrix<T, N> elements
     * @return Serie<T> Serie containing determinant values
     */
    template <typename T, std::size_t N> Serie<T> det(const Serie<FullMatrix<T, N>>& serie);

    /**
     * @brief Compute the determinant for each SymmetricMatrix in the Serie
     *
     * This function applies the determinant operation to each SymmetricMatrix element
     * in the input Serie, returning a Serie of scalar determinant values.
     *
     * @tparam T The element type (e.g., double, float)
     * @tparam N The matrix dimension (2 for 2x2, 3 for 3x3, etc.)
     * @param serie Input Serie containing SymmetricMatrix<T, N> elements
     * @return Serie<T> Serie containing determinant values
     */
    template <typename T, std::size_t N> Serie<T> det(const Serie<SymmetricMatrix<T, N>>& serie);

    /**
     * @brief Create a bound determinant function for FullMatrix Serie (for pipe operations)
     *
     * This function creates a callable that can be used in pipe operations to
     * compute determinants of FullMatrix elements in a Serie.
     *
     * @tparam T The element type (e.g., double, float)
     * @tparam N The matrix dimension
     * @return A callable that computes determinants of FullMatrix elements
     *
     * @example
     * ```cpp
     * Serie<FullMatrix<double, 3>> matrices = ...;
     * auto determinants = matrices | bind_det<double, 3>();
     * ```
     */
    template <typename T, std::size_t N> auto bind_det_fullmatrix();

    /**
     * @brief Create a bound determinant function for SymmetricMatrix Serie (for pipe operations)
     *
     * This function creates a callable that can be used in pipe operations to
     * compute determinants of SymmetricMatrix elements in a Serie.
     *
     * @tparam T The element type (e.g., double, float)
     * @tparam N The matrix dimension
     * @return A callable that computes determinants of SymmetricMatrix elements
     *
     * @example
     * ```cpp
     * Serie<SymmetricMatrix<double, 3>> matrices = ...;
     * auto determinants = matrices | bind_det<double, 3>();
     * ```
     */
    template <typename T, std::size_t N> auto bind_det_symmetric();

    /**
     * @brief Generic bind function that works with both matrix types
     *
     * This provides a unified interface for binding determinant operations
     * regardless of the matrix type (FullMatrix or SymmetricMatrix).
     *
     * @tparam MatrixType The matrix type (FullMatrix<T, N> or SymmetricMatrix<T, N>)
     * @return A callable that computes determinants
     */
    template <typename MatrixType> auto bind_det();

} // namespace df

#include "inline/det.hxx"