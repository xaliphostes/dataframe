/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
 * @return Serie with itemSize=3 containing point coordinates
 *
 * @example
 * ```cpp
 * // 1D line along x
 * auto line = df::grid::cartesian::from_dims({10}, {0,0,0}, {1,0,0});
 *
 * // 2D grid in xy plane
 * auto grid = df::grid::cartesian::from_dims({5,5}, {0,0,0}, {1,1,0});
 *
 * // 3D cube
 * auto cube = df::grid::cartesian::from_dims({3,3,3}, {0,0,0}, {1,1,1});
 * ```
 */
inline Serie from_dims(const std::vector<uint32_t> &npts,
                                 const Array &center = {0, 0, 0},
                                 const Array &dimensions = {1, 1, 1}) {
    if (npts.empty() || npts.size() > 3) {
        throw std::invalid_argument("npts must have 1, 2 or 3 components");
    }
    if (center.size() != 3 || dimensions.size() != 3) {
        throw std::invalid_argument(
            "center and dimensions must have 3 components");
    }

    // Calculate total number of points
    uint32_t total = 1;
    for (auto n : npts) {
        total *= n;
    }

    Serie result(3, total); // 3D points
    uint32_t index = 0;

    // Calculate spacing in each dimension
    Array spacing(3);
    for (size_t i = 0; i < npts.size(); ++i) {
        spacing[i] = npts[i] > 1 ? dimensions[i] / (npts[i] - 1) : 0;
    }

    // Fill remaining dimensions with 1
    std::vector<uint32_t> n = npts;
    n.resize(3, 1);

    // Generate points
    for (uint32_t k = 0; k < n[2]; ++k) {
        for (uint32_t j = 0; j < n[1]; ++j) {
            for (uint32_t i = 0; i < n[0]; ++i) {
                Array point = {center[0] - dimensions[0] / 2 + i * spacing[0],
                               center[1] - dimensions[1] / 2 + j * spacing[1],
                               center[2] - dimensions[2] / 2 + k * spacing[2]};
                result.set(index++, point);
            }
        }
    }

    return result;
}

/**
 * @brief Create a grid of points in 1D, 2D, or 3D given first and last points
 * @param npts Number of points in each dimension (1, 2, or 3 values)
 * @param p1 First point [x1,y1,z1]
 * @param p2 Opposite point [x2,y2,z2]
 * @return Serie with itemSize=3 containing point coordinates
 * @example
 * ```cpp
 * // 1D line from (0,0,0) to (1,0,0)
 * auto line = df::grid::cartesian::from_points({10}, {0,0,0}, {1,0,0});
 *
 * // 2D grid from (0,0,0) to (1,1,0)
 * auto grid = df::grid::cartesian::from_points({5,5}, {0,0,0}, {1,1,0});
 *
 * // 3D cube from (0,0,0) to (1,1,1)
 * auto cube = df::grid::cartesian::from_points({3,3,3}, {0,0,0}, {1,1,1});
 * ```
 */
Serie from_points(const std::vector<uint32_t> &npts, const Array &p1,
                            const Array &p2) {
    if (npts.empty() || npts.size() > 3) {
        throw std::invalid_argument("npts must have 1, 2 or 3 components");
    }
    if (p1.size() != 3 || p2.size() != 3) {
        throw std::invalid_argument("points must have 3 components");
    }

    uint32_t total = 1;
    for (auto n : npts) {
        total *= n;
    }

    Serie result(3, total);
    uint32_t index = 0;

    // Calculate spacing in each dimension
    Array delta(3);
    for (size_t i = 0; i < npts.size(); ++i) {
        delta[i] = npts[i] > 1 ? (p2[i] - p1[i]) / (npts[i] - 1) : 0;
    }

    // Fill remaining dimensions with 1
    std::vector<uint32_t> n = npts;
    n.resize(3, 1);

    // Generate points
    for (uint32_t k = 0; k < n[2]; ++k) {
        for (uint32_t j = 0; j < n[1]; ++j) {
            for (uint32_t i = 0; i < n[0]; ++i) {
                Array point = {p1[0] + i * delta[0], p1[1] + j * delta[1],
                               p1[2] + k * delta[2]};
                result.set(index++, point);
            }
        }
    }

    return result;
}

} // namespace cartesian
} // namespace grid
} // namespace df
