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
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>

namespace df {
namespace grid {

/**
 * @brief Structure to hold 3D grid information
 *
 * Working with attributes (data attributes associated with each grid point):
 * - get an attribute: `grid.attributes.get<T>(name)`
 * - remove an attribute: `grid.attributes.remove(name);`
 * - add a new attribute: `grid.attributes.add(name, values);`
 */
struct Grid3D {
    Vector3 origin;       // Origin point (x0, y0, z0)
    Vector3 spacing;      // Grid spacing (dx, dy, dz)
    iVector3 dimensions;  // Number of points in each direction (nx, ny, nz)
    Dataframe attributes; // Data attributes associated with each grid point

    /**
     * @brief Get grid point coordinates at given indices
     */
    Vector3 point_at(uint i, uint j, uint k) const;

    /**
     * @brief Get linear index from 3D indices
     */
    size_t linear_index(uint i, uint j, uint k) const;

    /**
     * @brief Get 3D indices from linear index
     */
    std::tuple<uint, uint, uint> grid_indices(size_t index) const;

    /**
     * @brief Get total number of points in the grid
     */
    size_t total_points() const;
};

} // namespace grid
} // namespace df

#include "inline/grid3d.hxx"