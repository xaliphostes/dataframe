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

#include <cmath>

namespace df {
namespace geo {

namespace details {

/**
 * @brief Find nearest neighbors and calculate interpolation weights
 * @param positions Series containing position vectors
 * @param query_point Point to interpolate at
 * @param indices Output vector of nearest neighbor indices
 * @param weights Output vector of interpolation weights
 */
template <typename T>
inline void find_nearest_neighbors(const GenSerie<T> &positions,
                            const std::vector<T> &query_point,
                            std::vector<uint32_t> &indices,
                            std::vector<T> &weights) {
    const uint32_t k = 4; // Number of nearest neighbors to use
    indices.resize(k);
    weights.resize(k);

    // Priority queue to find k nearest neighbors
    std::vector<std::pair<T, uint32_t>> distances;
    distances.reserve(positions.count());

    // Calculate distances to all points
    for (uint32_t i = 0; i < positions.count(); ++i) {
        auto pos = positions.array(i);
        T dist_sq = 0;

        for (uint32_t j = 0; j < query_point.size(); ++j) {
            T diff = pos[j] - query_point[j];
            dist_sq += diff * diff;
        }

        distances.emplace_back(std::sqrt(dist_sq), i);
    }

    // Sort to find k nearest
    std::partial_sort(
        distances.begin(), distances.begin() + k, distances.end(),
        [](const auto &a, const auto &b) { return a.first < b.first; });

    // Calculate inverse distance weights
    T weight_sum = 0;
    for (uint32_t i = 0; i < k; ++i) {
        indices[i] = distances[i].second;
        weights[i] =
            distances[i].first < 1e-10 ? 1.0 : 1.0 / distances[i].first;
        weight_sum += weights[i];
    }

    // Normalize weights
    for (auto &w : weights) {
        w /= weight_sum;
    }
}

} // namespace details

/**
 * @brief Interpolates field values at arbitrary points using linear
 * interpolation
 * @param field The field values serie (can be scalar or vector)
 * @param positions The positions serie where field values are known
 * @param query_points The points where we want to interpolate values
 * @return GenSerie<T> Interpolated values at query points
 */
template <typename T>
inline GenSerie<T> interpolate(const GenSerie<T> &field, const GenSerie<T> &positions,
                        const GenSerie<T> &query_points) {
    // Validate input dimensions
    if (positions.dimension() != query_points.dimension()) {
        throw std::invalid_argument(
            "Position and query points must have same dimension");
    }
    if (positions.count() != field.count()) {
        throw std::invalid_argument("Field and positions must have same count");
    }

    // Create result serie with same itemSize as field
    GenSerie<T> result(field.itemSize(), query_points.count());

    // For each query point
    for (uint32_t i = 0; i < query_points.count(); ++i) {
        auto query_point = query_points.array(i);

        // Find nearest neighbors and weights
        std::vector<uint32_t> nearest_indices;
        std::vector<T> weights;
        details::find_nearest_neighbors(positions, query_point, nearest_indices,
                               weights);

        // Interpolate value
        if (field.itemSize() == 1) {
            // Scalar field interpolation
            T interpolated_value = 0;
            T weight_sum = 0;

            for (size_t j = 0; j < nearest_indices.size(); ++j) {
                interpolated_value +=
                    weights[j] * field.value(nearest_indices[j]);
                weight_sum += weights[j];
            }

            result.setValue(i, interpolated_value / weight_sum);
        } else {
            // Vector field interpolation
            std::vector<T> interpolated_value(field.itemSize(), 0);
            T weight_sum = 0;

            for (size_t j = 0; j < nearest_indices.size(); ++j) {
                auto field_value = field.array(nearest_indices[j]);
                for (uint32_t k = 0; k < field.itemSize(); ++k) {
                    interpolated_value[k] += weights[j] * field_value[k];
                }
                weight_sum += weights[j];
            }

            for (auto &val : interpolated_value) {
                val /= weight_sum;
            }

            result.setArray(i, interpolated_value);
        }
    }

    return result;
}

// Helper function to create an interpolation operation
template <typename T> inline auto make_interpolate(const GenSerie<T> &positions) {
    return [&positions](const GenSerie<T> &field,
                        const GenSerie<T> &query_points) {
        return interpolate(field, positions, query_points);
    };
}

} // namespace geo
} // namespace df