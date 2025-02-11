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
#include <Eigen/Dense>
#include <array>
#include <cmath>
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Structure to hold grid information
 */
struct Grid2D {
    Vector2 origin;      // Origin point (x0, y0)
    Vector2 spacing;     // Grid spacing (dx, dy)
    iVector2 dimensions; // Number of points in each direction (nx, ny)

    /**
     * @brief Get grid point coordinates at given indices
     */
    Vector2 point_at(uint i, uint j) const {
        return {origin[0] + i * spacing[0], origin[1] + j * spacing[1]};
    }

    /**
     * @brief Get linear index from 2D indices
     */
    size_t linear_index(uint i, uint j) const { return i + j * dimensions[0]; }

    /**
     * @brief Get 2D indices from linear index
     */
    std::pair<uint, uint> grid_indices(size_t index) const {
        uint i = index % dimensions[0];
        uint j = index / dimensions[0];
        return {i, j};
    }

    /**
     * @brief Get total number of points in the grid
     */
    size_t total_points() const { return dimensions[0] * dimensions[1]; }
};

}