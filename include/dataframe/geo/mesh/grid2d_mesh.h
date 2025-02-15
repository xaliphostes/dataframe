/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/geo/mesh/mesh.h>
#include <dataframe/geo/types.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/geo/grid/from_points.h>
#include <stdexcept>

namespace df {

/**
 * @brief Class representing a triangulated surface mesh in 2D or 3D
 * @tparam N Dimension of the mesh (2 or 3)
 * @param n Number of points per side
 * @param size Size of the square side length
 */
Mesh2D generate_grid2d_mesh(size_t n = 51, double size = 10) {
    // Generate grid points
    iVector2 npts{static_cast<unsigned int>(n), static_cast<unsigned int>(n)};
    Vector2 center = {0.0, 0.0};
    Vector2 dimensions = {size, size};

    auto positions = grid::cartesian::from_dims(npts, center, dimensions);

    // Generate triangles
    std::vector<iVector3> triangles;
    triangles.reserve((n - 1) * (n - 1) * 2); // 2 triangles per grid cell

    for (uint j = 0; j < n - 1; ++j) {
        for (uint i = 0; i < n - 1; ++i) {
            // Calculate vertex indices for the current grid cell
            uint v0 = i + j * n;
            uint v1 = (i + 1) + j * n;
            uint v2 = i + (j + 1) * n;
            uint v3 = (i + 1) + (j + 1) * n;

            // Add two triangles for the current grid cell
            triangles.push_back({v0, v1, v2});
            triangles.push_back({v1, v3, v2});
        }
    }

    // Create mesh
    return Mesh2D(Serie<Vector2>(positions.asArray()),
                  Serie<iVector3>(triangles));
}

} // namespace df
