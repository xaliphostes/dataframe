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

    Vector3 point_at(uint i, uint j, uint k) const {
        return {origin[0] + i * spacing[0], origin[1] + j * spacing[1],
                origin[2] + k * spacing[2]};
    }

    size_t linear_index(uint i, uint j, uint k) const {
        return i + j * dimensions[0] + k * dimensions[0] * dimensions[1];
    }

    std::tuple<uint, uint, uint> grid_indices(size_t index) const {
        uint i = index % dimensions[0];
        uint j = (index / dimensions[0]) % dimensions[1];
        uint k = index / (dimensions[0] * dimensions[1]);
        return {i, j, k};
    }

    size_t total_points() const {
        return dimensions[0] * dimensions[1] * dimensions[2];
    }
};

} // namespace df