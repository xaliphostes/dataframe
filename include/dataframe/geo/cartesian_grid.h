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

// grid.h
#pragma once
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>
#include <vector>

namespace df {
namespace grid {
namespace cartesian {

/**
 * Generate a regular grid of points centered at a given position with specified
 * dimensions
 * @param npts Number of points in each dimension
 * @param center Center position of the grid
 * @param dimensions Total dimensions of the grid in each direction
 * @return Serie of grid point positions
 */
template <size_t N>
Serie<Vector<N>> from_dims(const iVector<N> &npts, const Vector<N> &center,
                           const Vector<N> &dimensions);

/**
 * Generate a regular grid of points between two corner points
 * @param npts Number of points in each dimension
 * @param p1 First corner point
 * @param p2 Second corner point
 * @return Serie of grid point positions
 */
template <size_t N>
Serie<Vector<N>> from_points(const iVector<N> &npts,
                             const std::vector<double> &p1,
                             const std::vector<double> &p2);

} // namespace cartesian
} // namespace grid
} // namespace df

#include "inline/cartesian_grid.hxx"