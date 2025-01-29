#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/kdtree.h>

namespace df {
namespace geo {

/**
 * @brief Compute divergence of a vector field
 * @param field Vector field (itemSize must match coordinates dimension)
 * @param coordinates Point coordinates (itemSize 2 or 3)
 * @return Serie<T> Divergence scalar field (itemSize = 1)
 */
/**
 * @brief Compute divergence of a vector field using KDTree for neighbor search
 * @param field Vector field (itemSize must match coordinates dimension)
 * @param coordinates Point coordinates (itemSize 2 or 3)
 * @param search_radius Radius for neighbor search
 * @return Serie<T> Divergence scalar field (itemSize = 1)
 */
template <typename T>
Serie<T> divergence(const Serie<T> &field, const Serie<T> &coordinates,
                       T search_radius) {
    // Validate input dimensions
    const uint32_t dim = coordinates.itemSize();
    if (dim != 2 && dim != 3) {
        throw std::invalid_argument("Coordinates must be 2D or 3D");
    }
    if (field.itemSize() != dim) {
        throw std::invalid_argument(
            "Field must have same dimension as coordinates");
    }
    if (field.count() != coordinates.count()) {
        throw std::invalid_argument(
            "Field and coordinates must have same count");
    }

    // Build KD-tree for efficient neighbor search
    KDTree<T> tree = build_kdtree(coordinates);
    const uint32_t num_points = coordinates.count();
    Serie<T> result(1, num_points);

// For each point, find nearby points using KDTree
#pragma omp parallel for if (num_points > 1000)
    for (uint32_t i = 0; i < num_points; ++i) {
        auto p = coordinates.array(i);
        auto v = field.array(i);

        // Find neighbors within search radius
        auto neighbor_indices = tree.find_radius(p, search_radius);

        // Initialize matrices for least squares computation
        std::vector<std::vector<T>> A(neighbor_indices.count(),
                                      std::vector<T>(dim));
        std::vector<std::vector<T>> b(dim,
                                      std::vector<T>(neighbor_indices.count()));

        // Fill matrices with neighbor data
        for (uint32_t j = 0; j < neighbor_indices.count(); ++j) {
            uint32_t neighbor_idx = neighbor_indices.value(j);
            if (neighbor_idx == i)
                continue;

            auto pj = coordinates.array(neighbor_idx);
            auto vj = field.array(neighbor_idx);

            // Compute relative position
            for (uint32_t k = 0; k < dim; ++k) {
                A[j][k] = pj[k] - p[k];
                b[k][j] = vj[k] - v[k];
            }
        }

        // Solve least squares problem for each component
        T divergence = 0;
        for (uint32_t k = 0; k < dim; ++k) {
            T sum_num = 0;
            T sum_denom = 0;

            for (uint32_t j = 0; j < neighbor_indices.count(); ++j) {
                if (std::abs(A[j][k]) > 1e-10) {
                    T weight = 1.0 / (A[j][k] * A[j][k]);
                    sum_num += weight * b[k][j] * A[j][k];
                    sum_denom += weight * A[j][k] * A[j][k];
                }
            }

            if (sum_denom > 1e-10) {
                divergence += sum_num / sum_denom;
            }
        }

        result.setValue(i, divergence);
    }

    return result;
}

/**
 * @brief Compute divergence using central differences on a regular grid
 * @param field Vector field (itemSize must be 2 or 3)
 * @param bounds Domain bounds {min_x, max_x, min_y, max_y[, min_z, max_z]}
 * @param resolution Grid resolution in each dimension
 * @return Serie<T> Divergence scalar field (itemSize = 1)
 */
template <typename T>
Serie<T> divergence(const Serie<T> &field,
                            const std::vector<T> &bounds,
                            const std::vector<uint32_t> &resolution) {
    const uint32_t dim = field.itemSize();
    if (dim != 2 && dim != 3) {
        throw std::invalid_argument("Field must be 2D or 3D");
    }
    if (bounds.size() != 2 * dim) {
        throw std::invalid_argument(
            "Bounds must specify min/max for each dimension");
    }
    if (resolution.size() != dim) {
        throw std::invalid_argument(
            "Resolution must specify size for each dimension");
    }

    // Compute grid spacing
    std::vector<T> dx(dim);
    for (uint32_t i = 0; i < dim; ++i) {
        dx[i] = (bounds[2 * i + 1] - bounds[2 * i]) / (resolution[i] - 1);
    }

    Serie<T> result(1, field.count());

    // Helper to convert flat index to grid indices
    auto flat_to_grid = [&](uint32_t idx) {
        std::vector<uint32_t> grid_idx(dim);
        if (dim == 2) {
            grid_idx[1] = idx / resolution[0];
            grid_idx[0] = idx % resolution[0];
        } else { // dim == 3
            grid_idx[2] = idx / (resolution[0] * resolution[1]);
            uint32_t remainder = idx % (resolution[0] * resolution[1]);
            grid_idx[1] = remainder / resolution[0];
            grid_idx[0] = remainder % resolution[0];
        }
        return grid_idx;
    };

    // Helper to convert grid indices to flat index
    auto grid_to_flat = [&](const std::vector<uint32_t> &grid_idx) {
        if (dim == 2) {
            return grid_idx[1] * resolution[0] + grid_idx[0];
        } else { // dim == 3
            return grid_idx[2] * resolution[0] * resolution[1] +
                   grid_idx[1] * resolution[0] + grid_idx[0];
        }
    };

// Compute divergence at each point using central differences
#pragma omp parallel for if (field.count() > 1000)
    for (uint32_t i = 0; i < field.count(); ++i) {
        auto grid_idx = flat_to_grid(i);
        T div = 0;

        for (uint32_t d = 0; d < dim; ++d) {
            if (grid_idx[d] > 0 && grid_idx[d] < resolution[d] - 1) {
                // Can use central difference
                auto idx_minus = grid_idx;
                auto idx_plus = grid_idx;
                idx_minus[d]--;
                idx_plus[d]++;

                uint32_t flat_minus = grid_to_flat(idx_minus);
                uint32_t flat_plus = grid_to_flat(idx_plus);

                div +=
                    (field.array(flat_plus)[d] - field.array(flat_minus)[d]) /
                    (2 * dx[d]);
            } else if (grid_idx[d] == 0) {
                // Forward difference at start
                auto idx_plus = grid_idx;
                idx_plus[d]++;
                uint32_t flat_plus = grid_to_flat(idx_plus);

                div += (field.array(flat_plus)[d] - field.array(i)[d]) / dx[d];
            } else {
                // Backward difference at end
                auto idx_minus = grid_idx;
                idx_minus[d]--;
                uint32_t flat_minus = grid_to_flat(idx_minus);

                div += (field.array(i)[d] - field.array(flat_minus)[d]) / dx[d];
            }
        }

        result.setValue(i, div);
    }

    return result;
}

// Helper function to create a divergence operation
template <typename T> auto make_divergence(const Serie<T> &coordinates) {
    return [&coordinates](const Serie<T> &field) {
        return divergence(field, coordinates);
    };
}

template <typename T>
auto make_divergence_grid(const std::vector<T> &bounds,
                          const std::vector<uint32_t> &resolution) {
    return [=](const Serie<T> &field) {
        return divergence(field, bounds, resolution);
    };
}

} // namespace geo
} // namespace df