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

/**
 * @brief Parameters for field interpolation
 */
struct InterpolationParams {
    size_t num_neighbors = 4; // Number of neighbors to use
    double power = 2.0;       // Power parameter for IDW
    SmoothingMethod smoothing = SmoothingMethod::None;
    double smoothing_radius = 0.1;   // Radius for smoothing kernel
    size_t smoothing_iterations = 1; // Number of smoothing passes
};

/**
 * @brief Interpolate a field (scalar or vector) using IDW with optional
 * smoothing
 *
 * @tparam T Type of field values (scalar or vector)
 * @tparam DIM Dimension of the space
 */
template <typename T, size_t DIM>
Serie<T> interpolate_field(
    const Serie<typename detail::point_type<DIM>::type> &points,
    const Serie<typename detail::point_type<DIM>::type> &reference_points,
    const Serie<T> &values,
    const InterpolationParams &params = InterpolationParams{}) {
    using point_t = typename detail::point_type<DIM>::type;

    if (points.empty() || reference_points.empty()) {
        throw std::runtime_error("Input series cannot be empty");
    }
    if (reference_points.size() != values.size()) {
        throw std::runtime_error(
            "Number of reference points must match number of values");
    }

    // Build KDTree for reference points
    Serie<size_t> indices(reference_points.size());
    std::iota(indices.begin(), indices.end(), 0);
    KDTree<size_t, DIM> kdtree(indices, reference_points);

    // First pass: Interpolate values
    auto interpolated = points.map([&](const point_t &point, size_t) -> T {
        auto neighbors =
            kdtree.findNearest(Serie<point_t>({point}), params.num_neighbors);

        if (kdtree.squaredDistance(neighbors[0].first, point) < 1e-10) {
            return values[neighbors[0].first];
        }

        // Initialize weighted sum variables
        T value_sum{};
        double weight_sum = 0.0;

        for (const auto &[idx, _] : neighbors) {
            double dist = std::sqrt(kdtree.squaredDistance(idx, point));
            double weight = 1.0 / std::pow(dist, params.power);

            if constexpr (std::is_arithmetic_v<T>) {
                value_sum += values[idx] * weight;
            } else {
                // For vector types, component-wise multiplication
                for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                    value_sum[i] += values[idx][i] * weight;
                }
            }
            weight_sum += weight;
        }

        // Normalize by weight sum
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
                // Find points within smoothing radius
                std::vector<size_t> neighbors;
                point_tree.findInRadius(point, params.smoothing_radius,
                                        neighbors);

                if (neighbors.empty()) {
                    return interpolated[idx];
                }

                if (params.smoothing == SmoothingMethod::Gaussian) {
                    // Gaussian weighted average
                    T sum{};
                    double weight_sum = 0.0;

                    for (size_t n_idx : neighbors) {
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

                    if constexpr (std::is_arithmetic_v<T>) {
                        return sum / weight_sum;
                    } else {
                        T result{};
                        for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                            result[i] = sum[i] / weight_sum;
                        }
                        return result;
                    }
                } else if (params.smoothing == SmoothingMethod::Mean) {
                    // Simple average
                    T sum{};
                    for (size_t n_idx : neighbors) {
                        if constexpr (std::is_arithmetic_v<T>) {
                            sum += interpolated[n_idx];
                        } else {
                            for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                                sum[i] += interpolated[n_idx][i];
                            }
                        }
                    }

                    if constexpr (std::is_arithmetic_v<T>) {
                        return sum / static_cast<double>(neighbors.size());
                    } else {
                        T result{};
                        for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                            result[i] =
                                sum[i] / static_cast<double>(neighbors.size());
                        }
                        return result;
                    }
                } else { // MedianFilter
                    if constexpr (std::is_arithmetic_v<T>) {
                        std::vector<T> values;
                        values.reserve(neighbors.size());
                        for (size_t n_idx : neighbors) {
                            values.push_back(interpolated[n_idx]);
                        }
                        std::sort(values.begin(), values.end());
                        return values[values.size() / 2];
                    } else {
                        // For vector fields, apply median filter component-wise
                        T result{};
                        for (size_t i = 0; i < std::tuple_size_v<T>; ++i) {
                            std::vector<double> component_values;
                            component_values.reserve(neighbors.size());
                            for (size_t n_idx : neighbors) {
                                component_values.push_back(
                                    interpolated[n_idx][i]);
                            }
                            std::sort(component_values.begin(),
                                      component_values.end());
                            result[i] =
                                component_values[component_values.size() / 2];
                        }
                        return result;
                    }
                }
            });
            interpolated = smoothed;
        }
    }

    return interpolated;
}

/**
 * @brief Bind function for use in pipelines
 */
template <typename T, size_t DIM>
auto bind_interpolate_field(
    const Serie<typename detail::point_type<DIM>::type> &reference_points,
    const Serie<T> &values,
    const InterpolationParams &params = InterpolationParams{}) {
    return [&](const Serie<typename detail::point_type<DIM>::type> &points) {
        return interpolate_field<T, DIM>(points, reference_points, values,
                                         params);
    };
}

} // namespace df