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
namespace grid {
namespace cartesian {

/**
 * @brief Create a grid of points in 1D, 2D, or 3D
 * @param npts Number of points in each dimension (1, 2, or 3 values)
 * @param center Center position of the grid [x,y,z]
 * @param dimensions Size of the grid in each dimension [dx,dy,dz]
 * @return Serie<T> with itemSize=3 containing point coordinates
 */
template <typename T>
Serie<T> from_dims(const std::vector<uint32_t> &npts,
                      const std::vector<T> &center = {0, 0, 0},
                      const std::vector<T> &dimensions = {1, 1, 1});

/**
 * @brief Create a grid of points in 1D, 2D, or 3D given first and last points
 * @param npts Number of points in each dimension (1, 2, or 3 values)
 * @param p1 First point [x1,y1,z1]
 * @param p2 Opposite point [x2,y2,z2]
 * @return Serie<T> with itemSize=3 containing point coordinates
 */
template <typename T>
Serie<T> from_points(const std::vector<uint32_t> &npts,
                        const std::vector<T> &p1, const std::vector<T> &p2);

} // namespace cartesian
} // namespace grid
} // namespace df

#include "inline/cartesian_grid.hxx"