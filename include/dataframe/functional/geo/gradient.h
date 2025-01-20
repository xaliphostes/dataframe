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

namespace df {
namespace geo {

/**
 * @brief Compute the gradient of a scalar field
 * @param field Scalar field (itemSize=1)
 * @param dx Spacing between points in each dimension
 * @return GenSerie<T> with itemSize=3 containing gradient vectors [∂f/∂x,
 * ∂f/∂y, ∂f/∂z]
 * @example
 * ```cpp
 * GenSerie<double> field(1, {...});  // Scalar field values
 * std::vector<double> dx = {1.0, 1.0, 1.0};  // Unit spacing
 * auto grad = gradient(field, {10,10,10}, dx);  // Gradient vectors
 * ```
 */
template <typename T>
GenSerie<T> gradient(const GenSerie<T> &field,
                     const std::vector<uint32_t> &grid_size,
                     const std::vector<T> &dx);

/**
 * @brief Create gradient operation that can be used with the pipe operator
 * @param grid_size Grid dimensions {nx,ny,nz}
 * @param dx Grid spacing {dx,dy,dz}
 * @code
 * // Create a simple 1D field with 3 points
 * df::GenSerie<double> field(1, {0.0, 1.0, 2.0});
 *
 * auto result = field |
 *  df::geo::make_gradient({3, 1, 1}, df::Serie::Array{1.0, 1.0, 1.0});
 * @endcode
 */
template <typename T>
auto make_gradient(const std::vector<uint32_t> &, const std::vector<T> &);

} // namespace geo
} // namespace df

#include "inline/gradient.hxx"