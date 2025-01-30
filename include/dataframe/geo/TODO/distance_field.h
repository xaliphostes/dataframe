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
#include <dataframe/meta.h>
#include <dataframe/geo/kdtree.h>

namespace df {
namespace geo {

/**
 * @brief Compute distance field from a set of points in 2D or 3D
 * @param points Input points (itemSize must be 2 or 3)
 * @param bounds Bounds of the domain {min_x, max_x, min_y, max_y[, min_z,
 * max_z]}
 * @param resolution Number of grid points in each dimension
 * @return Serie<T> Distance values on regular grid (itemSize=1)
 */
template <typename T>
Serie<T> distance_field(const Serie<T> &points,
                           const std::vector<T> &bounds,
                           const std::vector<uint32_t> &resolution) {
    // Validate input dimensions
    const uint32_t dim = points.itemSize();
    if (dim != 2 && dim != 3) {
        throw std::invalid_argument("Points must be 2D or 3D");
    }
    if (bounds.size() != 2 * dim) {
        throw std::invalid_argument(
            "Bounds must specify min/max for each dimension");
    }
    if (resolution.size() != dim) {
        throw std::invalid_argument(
            "Resolution must specify grid size for each dimension");
    }

    // Build KD-tree for efficient point queries
    auto kdtree = build_kdtree(points);

    // Compute grid spacing
    std::vector<T> spacing(dim);
    for (uint32_t i = 0; i < dim; ++i) {
        spacing[i] = (bounds[2 * i + 1] - bounds[2 * i]) / (resolution[i] - 1);
    }

    // Compute total number of grid points
    uint32_t total_points = 1;
    for (uint32_t i = 0; i < dim; ++i) {
        total_points *= resolution[i];
    }

    // Initialize result series for distances
    Serie<T> result(1, total_points);

    // Helper function to convert grid indices to coordinates
    auto index_to_coord = [&](const std::vector<uint32_t> &idx) {
        std::vector<T> coord(dim);
        for (uint32_t i = 0; i < dim; ++i) {
            coord[i] = bounds[2 * i] + idx[i] * spacing[i];
        }
        return coord;
    };

    // Helper function to convert flat index to grid indices
    auto flat_to_grid = [&](uint32_t flat_idx) {
        std::vector<uint32_t> idx(dim);
        if (dim == 2) {
            idx[1] = flat_idx / resolution[0];
            idx[0] = flat_idx % resolution[0];
        } else { // dim == 3
            idx[2] = flat_idx / (resolution[0] * resolution[1]);
            uint32_t remainder = flat_idx % (resolution[0] * resolution[1]);
            idx[1] = remainder / resolution[0];
            idx[0] = remainder % resolution[0];
        }
        return idx;
    };

// Compute distance at each grid point
#pragma omp parallel for if (total_points > 1000)
    for (uint32_t i = 0; i < total_points; ++i) {
        auto grid_idx = flat_to_grid(i);
        auto coord = index_to_coord(grid_idx);

        // Find nearest point
        auto nearest = kdtree.find_nearest(coord, 1);
        if (!nearest.isEmpty()) {
            auto point = points.array(nearest.value(0));

            // Compute Euclidean distance
            T dist_sq = 0;
            for (uint32_t j = 0; j < dim; ++j) {
                T diff = coord[j] - point[j];
                dist_sq += diff * diff;
            }
            result.setValue(i, std::sqrt(dist_sq));
        } else {
            // No points found (shouldn't happen with valid input)
            result.setValue(i, std::numeric_limits<T>::max());
        }
    }

    return result;
}

/**
 * @brief Compute signed distance field from points and normals in 2D or 3D
 * @param points Input points (itemSize must be 2 or 3)
 * @param normals Point normals (same dimension as points)
 * @param bounds Bounds of the domain {min_x, max_x, min_y, max_y[, min_z,
 * max_z]}
 * @param resolution Number of grid points in each dimension
 * @return Serie<T> Signed distance values on regular grid (itemSize=1)
 */
template <typename T>
Serie<T> signed_distance_field(const Serie<T> &points,
                                  const Serie<T> &normals,
                                  const std::vector<T> &bounds,
                                  const std::vector<uint32_t> &resolution) {
    // Validate input dimensions
    const uint32_t dim = points.itemSize();
    if (dim != 2 && dim != 3) {
        throw std::invalid_argument("Points must be 2D or 3D");
    }
    if (normals.itemSize() != dim) {
        throw std::invalid_argument(
            "Normals must have same dimension as points");
    }
    if (normals.count() != points.count()) {
        throw std::invalid_argument(
            "Must have same number of normals as points");
    }

    // First compute unsigned distance field
    auto distances = distance_field(points, bounds, resolution);

    // Helper for dot product
    auto dot = [](const std::vector<T> &a, const std::vector<T> &b) {
        T result = 0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[i];
        }
        return result;
    };

    // Build KD-tree for point queries
    auto kdtree = build_kdtree(points);

    // Helper function to convert grid indices to coordinates (same as above)
    auto index_to_coord = [&](const std::vector<uint32_t> &idx) {
        std::vector<T> coord(dim);
        for (uint32_t i = 0; i < dim; ++i) {
            coord[i] =
                bounds[2 * i] + idx[i] * ((bounds[2 * i + 1] - bounds[2 * i]) /
                                          (resolution[i] - 1));
        }
        return coord;
    };

    // Helper function to convert flat index to grid indices (same as above)
    auto flat_to_grid = [&](uint32_t flat_idx) {
        std::vector<uint32_t> idx(dim);
        if (dim == 2) {
            idx[1] = flat_idx / resolution[0];
            idx[0] = flat_idx % resolution[0];
        } else { // dim == 3
            idx[2] = flat_idx / (resolution[0] * resolution[1]);
            uint32_t remainder = flat_idx % (resolution[0] * resolution[1]);
            idx[1] = remainder / resolution[0];
            idx[0] = remainder % resolution[0];
        }
        return idx;
    };

// Determine sign at each grid point
#pragma omp parallel for if (distances.count() > 1000)
    for (uint32_t i = 0; i < distances.count(); ++i) {
        auto grid_idx = flat_to_grid(i);
        auto coord = index_to_coord(grid_idx);

        // Find nearest point
        auto nearest = kdtree.find_nearest(coord, 1);
        if (!nearest.isEmpty()) {
            uint32_t idx = nearest.value(0);
            auto point = points.array(idx);
            auto normal = normals.array(idx);

            // Compute vector from point to grid point
            std::vector<T> to_grid(dim);
            for (uint32_t j = 0; j < dim; ++j) {
                to_grid[j] = coord[j] - point[j];
            }

            // Sign is determined by dot product with normal
            T sign = dot(to_grid, normal) >= 0 ? 1 : -1;
            distances.setValue(i, sign * distances.value(i));
        }
    }

    return distances;
}

// Helper function to create a distance field operation
template <typename T>
auto make_distance_field(const std::vector<T> &bounds,
                         const std::vector<uint32_t> &resolution,
                         bool signed_field = false) {
    return [=](const Serie<T> &points,
               const Serie<T> &normals = Serie<T>()) {
        if (signed_field) {
            if (normals.isEmpty()) {
                throw std::invalid_argument(
                    "Normals required for signed distance field");
            }
            return signed_distance_field(points, normals, bounds, resolution);
        }
        return distance_field(points, bounds, resolution);
    };
}

} // namespace geo
} // namespace df
