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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/utils/kdtree.h>
#include <dataframe/types.h>

namespace df {

// Smoothing methods available
enum class SmoothingMethod {
    None,        // No smoothing
    Gaussian,    // Gaussian kernel smoothing
    Mean,        // Moving average
    MedianFilter // Median filter
};

struct GroupInfo {
    size_t group_id;     // Identifier for the group
    double weight;       // Weight for this group
};

/**
 * @brief Represents a curved discontinuity using cubic spline
 */
struct CurvedDiscontinuity {
    std::vector<Vector2> control_points; // Control points defining the curve
    size_t num_segments = 50;            // Number of segments for approximation
    bool bidirectional = true; // Whether discontinuity affects both sides

    // Get points along the curve at parameter t ∈ [0,1]
    Vector2 evaluate(double t) const {
        if (control_points.size() < 2)
            return Vector2{};

        // Ensure t is in [0,1]
        t = std::max(0.0, std::min(1.0, t));

        // Convert t to segment index
        double scaled_t = t * (control_points.size() - 1);
        size_t i = static_cast<size_t>(scaled_t);
        if (i >= control_points.size() - 1)
            i = control_points.size() - 2;

        double local_t = scaled_t - i;

        // Cubic interpolation between points i and i+1
        Vector2 p0 = i > 0 ? control_points[i - 1] : control_points[i];
        Vector2 p1 = control_points[i];
        Vector2 p2 = control_points[i + 1];
        Vector2 p3 = i < control_points.size() - 2 ? control_points[i + 2] : p2;

        // Catmull-Rom spline coefficients
        double t2 = local_t * local_t;
        double t3 = t2 * local_t;

        Vector2 result;
        for (size_t dim = 0; dim < 2; ++dim) {
            result[dim] =
                0.5f *
                ((2.0f * p1[dim]) + (-p0[dim] + p2[dim]) * local_t +
                 (2.0f * p0[dim] - 5.0f * p1[dim] + 4.0f * p2[dim] - p3[dim]) *
                     t2 +
                 (-p0[dim] + 3.0f * p1[dim] - 3.0f * p2[dim] + p3[dim]) * t3);
        }
        return result;
    }

    // Check if a line segment intersects the curve
    bool intersects(const Vector2 &p1, const Vector2 &p2) const {
        if (control_points.size() < 2)
            return false;

        // Sample points along the curve
        std::vector<Vector2> curve_points;
        curve_points.reserve(num_segments + 1);

        for (size_t i = 0; i <= num_segments; ++i) {
            double t = static_cast<double>(i) / num_segments;
            curve_points.push_back(evaluate(t));
        }

        // Check each segment of the curve for intersection
        for (size_t i = 0; i < curve_points.size() - 1; ++i) {
            const Vector2 &a = curve_points[i];
            const Vector2 &b = curve_points[i + 1];

            // Line segment intersection check
            double denom = (b[0] - a[0]) * (p2[1] - p1[1]) -
                           (b[1] - a[1]) * (p2[0] - p1[0]);
            if (std::abs(denom) < 1e-10)
                continue; // Lines are parallel

            double t = ((a[1] - p1[1]) * (p2[0] - p1[0]) -
                        (a[0] - p1[0]) * (p2[1] - p1[1])) /
                       denom;
            double u = ((b[0] - a[0]) * (a[1] - p1[1]) -
                        (b[1] - a[1]) * (a[0] - p1[0])) /
                       denom;

            if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
                return true;
            }
        }
        return false;
    }
};

struct InterpolationParams {
    size_t num_neighbors = 4; // Number of neighbors to use
    double power = 2.0;       // Power parameter for IDW
    SmoothingMethod smoothing = SmoothingMethod::None;
    double smoothing_radius = 0.1;   // Radius for smoothing kernel
    size_t smoothing_iterations = 1; // Number of smoothing passes
    std::vector<CurvedDiscontinuity> discontinuities;
    bool respect_groups = false; // Whether to interpolate within groups only
    double cross_group_penalty = 1.0; // Penalty factor for crossing groups
};

/**
 * @brief Interpolate a field with support for curved discontinuities and groups
 */
template <typename T, size_t DIM>
Serie<T> interpolate_field(
    const Serie<typename detail::point_type<DIM>::type> &points,
    const Serie<typename detail::point_type<DIM>::type> &reference_points,
    const Serie<T> &values, const Serie<GroupInfo> &group_info,
    const InterpolationParams &params = InterpolationParams{}) {
    using point_t = typename detail::point_type<DIM>::type;

    if (points.empty() || reference_points.empty()) {
        throw std::runtime_error("Input series cannot be empty");
    }
    if (reference_points.size() != values.size()) {
        throw std::runtime_error(
            "Number of reference points must match number of values");
    }
    if (reference_points.size() != group_info.size()) {
        throw std::runtime_error(
            "Number of reference points must match group info size");
    }

    // Build KDTree for reference points
    Serie<size_t> indices(reference_points.size());
    std::iota(indices.begin(), indices.end(), 0);
    KDTree<size_t, DIM> kdtree(indices, reference_points);

    // First pass: Interpolate values with group awareness and discontinuities
    auto interpolated = points.map([&](const point_t &point, size_t) -> T {
        // Get more neighbors than needed to account for filtering
        auto neighbors = kdtree.findNearest(Serie<point_t>({point}),
                                            params.num_neighbors * 2);

        T value_sum{};
        double weight_sum = 0.0;
        size_t valid_neighbors = 0;
        Vector2 point2d{point[0],
                        point[1]}; // Project to 2D for discontinuity check

        for (const auto &[idx, _] : neighbors) {
            if (valid_neighbors >= params.num_neighbors)
                break;

            double dist = std::sqrt(kdtree.squaredDistance(idx, point));
            if (dist < 1e-10)
                return values[idx]; // Point coincides with reference point

            // Check for discontinuity crossing
            Vector2 ref2d{reference_points[idx][0], reference_points[idx][1]};
            bool crosses_discontinuity = false;

            for (const auto &disc : params.discontinuities) {
                if (disc.intersects(point2d, ref2d)) {
                    crosses_discontinuity = true;
                    break;
                }
            }

            if (crosses_discontinuity)
                continue;

            // Calculate weight with group influence
            double weight = 1.0 / std::pow(dist, params.power);
            if (params.respect_groups) {
                weight *= group_info[idx].weight;
            }

            // Accumulate weighted value
            if constexpr (std::is_arithmetic_v<T>) {
                value_sum += values[idx] * weight;
            } else {
                for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                    value_sum[i] += values[idx][i] * weight;
                }
            }
            weight_sum += weight;
            valid_neighbors++;
        }

        // Handle case with no valid neighbors
        if (weight_sum < 1e-10)
            return T{};

        // Normalize result
        if constexpr (std::is_arithmetic_v<T>) {
            return value_sum / weight_sum;
        } else {
            T result{};
            for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                result[i] = value_sum[i] / weight_sum;
            }
            return result;
        }
    });

    // Apply smoothing if requested
    if (params.smoothing != SmoothingMethod::None) {
        // Build KDTree for interpolated points
        Serie<size_t> point_indices(points.size());
        std::iota(point_indices.begin(), point_indices.end(), 0);
        KDTree<size_t, DIM> point_tree(point_indices, points);

        // Perform smoothing iterations
        auto smoothed = interpolated;
        for (size_t iter = 0; iter < params.smoothing_iterations; ++iter) {
            smoothed = points.map([&](const point_t &point, size_t idx) -> T {
                std::vector<size_t> neighbors;
                point_tree.findInRadius(point, params.smoothing_radius,
                                        neighbors);

                if (neighbors.empty()) {
                    return interpolated[idx];
                }

                Vector2 point2d{point[0], point[1]};

                if (params.smoothing == SmoothingMethod::Gaussian) {
                    T sum{};
                    double weight_sum = 0.0;

                    for (size_t n_idx : neighbors) {
                        // Check discontinuities
                        Vector2 neigh2d{points[n_idx][0], points[n_idx][1]};
                        bool crosses_discontinuity = false;
                        for (const auto &disc : params.discontinuities) {
                            if (disc.intersects(point2d, neigh2d)) {
                                crosses_discontinuity = true;
                                break;
                            }
                        }
                        if (crosses_discontinuity)
                            continue;

                        double dist =
                            std::sqrt(point_tree.squaredDistance(n_idx, point));
                        double weight =
                            std::exp(-dist * dist /
                                     (2.0 * params.smoothing_radius *
                                      params.smoothing_radius));

                        if constexpr (std::is_arithmetic_v<T>) {
                            sum += interpolated[n_idx] * weight;
                        } else {
                            for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                                sum[i] += interpolated[n_idx][i] * weight;
                            }
                        }
                        weight_sum += weight;
                    }

                    if (weight_sum < 1e-10)
                        return interpolated[idx];

                    if constexpr (std::is_arithmetic_v<T>) {
                        return sum / weight_sum;
                    } else {
                        T result{};
                        for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                            result[i] = sum[i] / weight_sum;
                        }
                        return result;
                    }
                }
                // Add other smoothing methods here if needed
                return interpolated[idx];
            });
            interpolated = smoothed;
        }
    }

    return interpolated;
}

} // namespace df