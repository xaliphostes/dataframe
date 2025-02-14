

/**
 * @brief RBF interpolation optimized for regular 2D grids
 * @param grid Grid information
 * @param values Serie of values at grid points (must match grid dimensions)
 * @param targets Serie of points to interpolate to
 * @param kernel RBF kernel type
 * @param epsilon Shape parameter for the kernel
 * @param support_radius Number of grid cells to consider for local
 * interpolation
 * @return Serie of interpolated values
 */

#include "grid/grid2d.h"
#include "grid/grid3d.h"
#include <Eigen/Dense>
#include <cmath>
#include <functional>

namespace df {

template <typename T>
inline Serie<T> rbf_grid_2d(const Grid2D &grid, const Serie<T> &values,
                     const Serie<Vector2> &targets, RBFKernel kernel,
                     double epsilon, uint support_radius) {
    if (values.size() != grid.total_points()) {
        throw std::runtime_error("Values size does not match grid dimensions");
    }

    auto kernel_fn = get_kernel_function(kernel);
    Serie<T> result(targets.size());

    // Precompute maximum distance for support radius
    double max_support_dist =
        support_radius * std::max(grid.spacing[0], grid.spacing[1]);
    max_support_dist *= max_support_dist; // Square it for comparison

// For each target point
#pragma omp parallel for if (targets.size() > 1000)
    for (size_t t = 0; t < targets.size(); ++t) {
        const auto &target = targets[t];

        // Find nearest grid point
        int i0 = std::round((target[0] - grid.origin[0]) / grid.spacing[0]);
        int j0 = std::round((target[1] - grid.origin[1]) / grid.spacing[1]);

        // Collect nearby points within support radius
        std::vector<Vector2> local_points;
        std::vector<T> local_values;

        for (int j = std::max(0, j0 - support_radius);
             j <= std::min(static_cast<int>(grid.dimensions[1] - 1),
                           j0 + support_radius);
             ++j) {
            for (int i = std::max(0, i0 - support_radius);
                 i <= std::min(static_cast<int>(grid.dimensions[0] - 1),
                               i0 + support_radius);
                 ++i) {

                Vector2 point = grid.point_at(i, j);
                double dist_sq = distance_squared_2d(target, point);

                if (dist_sq <= max_support_dist) {
                    local_points.push_back(point);
                    local_values.push_back(values[grid.linear_index(i, j)]);
                }
            }
        }

        // Local RBF interpolation
        const size_t n = local_points.size();
        if (n == 0) {
            // Point is outside the grid - use nearest value
            result[t] = values[grid.linear_index(
                std::clamp(i0, 0, static_cast<int>(grid.dimensions[0] - 1)),
                std::clamp(j0, 0, static_cast<int>(grid.dimensions[1] - 1)))];
            continue;
        }

        Eigen::MatrixXd A(n, n);
        Eigen::VectorXd b(n);

        // Build local interpolation matrix
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                double r = std::sqrt(
                    distance_squared_2d(local_points[i], local_points[j]));
                A(i, j) = kernel_fn(r, epsilon);
            }
            A(i, i) += 1e-10; // Small regularization
            b(i) = static_cast<double>(local_values[i]);
        }

        // Solve local system
        Eigen::VectorXd weights = A.ldlt().solve(b);

        // Compute interpolated value
        double sum = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double r = std::sqrt(distance_squared_2d(target, local_points[i]));
            sum += weights(i) * kernel_fn(r, epsilon);
        }

        result[t] = static_cast<T>(sum);
    }

    return result;
}

template <typename T>
inline Serie<T> rbf_grid_3d(const Grid3D &grid, const Serie<T> &values,
                     const Serie<Vector3> &targets, RBFKernel kernel,
                     double epsilon, uint support_radius) {
    if (values.size() != grid.total_points()) {
        throw std::runtime_error("Values size does not match grid dimensions");
    }

    auto kernel_fn = get_kernel_function(kernel);
    Serie<T> result(targets.size());

    // Precompute maximum distance for support radius
    double max_support_dist =
        support_radius *
        std::max({grid.spacing[0], grid.spacing[1], grid.spacing[2]});
    max_support_dist *= max_support_dist;

// For each target point
#pragma omp parallel for if (targets.size() > 1000)
    for (size_t t = 0; t < targets.size(); ++t) {
        const auto &target = targets[t];

        // Find nearest grid point
        int i0 = std::round((target[0] - grid.origin[0]) / grid.spacing[0]);
        int j0 = std::round((target[1] - grid.origin[1]) / grid.spacing[1]);
        int k0 = std::round((target[2] - grid.origin[2]) / grid.spacing[2]);

        // Collect nearby points within support radius
        std::vector<Vector3> local_points;
        std::vector<T> local_values;

        for (int k = std::max(0, k0 - support_radius);
             k <= std::min(static_cast<int>(grid.dimensions[2] - 1),
                           k0 + support_radius);
             ++k) {
            for (int j = std::max(0, j0 - support_radius);
                 j <= std::min(static_cast<int>(grid.dimensions[1] - 1),
                               j0 + support_radius);
                 ++j) {
                for (int i = std::max(0, i0 - support_radius);
                     i <= std::min(static_cast<int>(grid.dimensions[0] - 1),
                                   i0 + support_radius);
                     ++i) {

                    Vector3 point = grid.point_at(i, j, k);
                    double dist_sq = distance_squared_3d(target, point);

                    if (dist_sq <= max_support_dist) {
                        local_points.push_back(point);
                        local_values.push_back(
                            values[grid.linear_index(i, j, k)]);
                    }
                }
            }
        }

        // Local RBF interpolation
        const size_t n = local_points.size();
        if (n == 0) {
            // Point is outside the grid - use nearest value
            result[t] = values[grid.linear_index(
                std::clamp(i0, 0, static_cast<int>(grid.dimensions[0] - 1)),
                std::clamp(j0, 0, static_cast<int>(grid.dimensions[1] - 1)),
                std::clamp(k0, 0, static_cast<int>(grid.dimensions[2] - 1)))];
            continue;
        }

        Eigen::MatrixXd A(n, n);
        Eigen::VectorXd b(n);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                double r = std::sqrt(
                    distance_squared_3d(local_points[i], local_points[j]));
                A(i, j) = kernel_fn(r, epsilon);
            }
            A(i, i) += 1e-10;
            b(i) = static_cast<double>(local_values[i]);
        }

        Eigen::VectorXd weights = A.ldlt().solve(b);

        double sum = 0.0;
        for (size_t i = 0; i < n; ++i) {
            double r = std::sqrt(distance_squared_3d(target, local_points[i]));
            sum += weights(i) * kernel_fn(r, epsilon);
        }

        result[t] = static_cast<T>(sum);
    }

    return result;
}

} // namespace df