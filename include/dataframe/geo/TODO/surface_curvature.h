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
#include <dataframe/meta.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace df {
namespace geo {

/**
 * @brief Compute mean curvature on a triangulated surface using a discrete
 * Laplacian Mean curvature H = ||LX||/2A where L is the discrete
 * Laplace-Beltrami operator
 * @param positions Vertex positions (itemSize must be 3)
 * @param indices Triangle indices (itemSize must be 3)
 * @return Serie<T> Mean curvature at each vertex (scalar field)
 */
template <typename T>
Serie<T> surface_curvature(const Serie<T> &positions,
                              const Serie<uint32_t> &indices) {
    // Validate input dimensions
    if (positions.itemSize() != 3) {
        throw std::invalid_argument(
            "Positions must be 3D (itemSize must be 3)");
    }
    if (indices.itemSize() != 3) {
        throw std::invalid_argument(
            "Indices must represent triangles (itemSize must be 3)");
    }

    const uint32_t num_vertices = positions.count();
    const uint32_t num_triangles = indices.count();

    auto vec_add = [](const std::vector<T>& a, const std::vector<T>& b) {
        return std::vector<T>{a[0]+b[0], a[1]+b[1], a[2]+b[2]};
    };

    // Helper functions
    auto vec_sub = [](const std::vector<T> &a, const std::vector<T> &b) {
        return std::vector<T>{a[0] - b[0], a[1] - b[1], a[2] - b[2]};
    };

    auto vec_scale = [](const std::vector<T> &a, T scale) {
        return std::vector<T>{a[0] * scale, a[1] * scale, a[2] * scale};
    };

    auto dot = [](const std::vector<T> &a, const std::vector<T> &b) {
        return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    };

    auto cross = [](const std::vector<T> &a, const std::vector<T> &b) {
        return std::vector<T>{a[1] * b[2] - a[2] * b[1],
                              a[2] * b[0] - a[0] * b[2],
                              a[0] * b[1] - a[1] * b[0]};
    };

    auto length = [dot](const std::vector<T> &v) { return std::sqrt(dot(v, v)); };

    // Structure to store per-vertex data
    struct VertexData {
        std::unordered_set<uint32_t> neighbor_vertices;
        T mixed_area = 0.0;
        std::vector<T> laplacian{0.0, 0.0, 0.0};
    };
    std::vector<VertexData> vertex_data(num_vertices);

    // First pass: collect neighbors and compute mixed areas
    for (uint32_t i = 0; i < num_triangles; ++i) {
        auto tri = indices.array(i);
        uint32_t v0 = tri[0], v1 = tri[1], v2 = tri[2];

        // Store vertex neighbors
        vertex_data[v0].neighbor_vertices.insert(v1);
        vertex_data[v0].neighbor_vertices.insert(v2);
        vertex_data[v1].neighbor_vertices.insert(v0);
        vertex_data[v1].neighbor_vertices.insert(v2);
        vertex_data[v2].neighbor_vertices.insert(v0);
        vertex_data[v2].neighbor_vertices.insert(v1);

        // Compute triangle area
        auto p0 = positions.array(v0);
        auto p1 = positions.array(v1);
        auto p2 = positions.array(v2);

        auto e01 = vec_sub(p1, p0);
        auto e02 = vec_sub(p2, p0);
        auto e12 = vec_sub(p2, p1);

        T area = length(cross(e01, e02)) * 0.5;

        // Compute angles
        T len01 = length(e01);
        T len02 = length(e02);
        T len12 = length(e12);

        T angle0 = std::acos(dot(e01, e02) / (len01 * len02));
        T angle1 = std::acos(dot(vec_scale(e01, -1.0), e12) / (len01 * len12));
        T angle2 = M_PI - angle0 - angle1;

        // Add mixed area contribution (Voronoi)
        if (angle0 < M_PI_2 && angle1 < M_PI_2 && angle2 < M_PI_2) {
            // Use Voronoi area
            T cot1 = std::cos(angle1) / std::sin(angle1);
            T cot2 = std::cos(angle2) / std::sin(angle2);
            vertex_data[v0].mixed_area +=
                (len01 * len01 * cot2 + len02 * len02 * cot1) / 8.0;

            T cot0 = std::cos(angle0) / std::sin(angle0);
            vertex_data[v1].mixed_area +=
                (len01 * len01 * cot2 + len12 * len12 * cot0) / 8.0;

            vertex_data[v2].mixed_area +=
                (len02 * len02 * cot1 + len12 * len12 * cot0) / 8.0;
        } else {
            // Use barycentric area for obtuse triangles
            vertex_data[v0].mixed_area += area / 3.0;
            vertex_data[v1].mixed_area += area / 3.0;
            vertex_data[v2].mixed_area += area / 3.0;
        }
    }

    // Second pass: compute mean curvature normal using discrete Laplacian
    Serie<T> result(1, num_vertices);

    for (uint32_t i = 0; i < num_triangles; ++i) {
        auto tri = indices.array(i);
        uint32_t v0 = tri[0], v1 = tri[1], v2 = tri[2];

        auto p0 = positions.array(v0);
        auto p1 = positions.array(v1);
        auto p2 = positions.array(v2);

        // Compute edge vectors
        auto e01 = vec_sub(p1, p0);
        auto e02 = vec_sub(p2, p0);
        auto e12 = vec_sub(p2, p1);

        // Compute angles
        T len01 = length(e01);
        T len02 = length(e02);
        T len12 = length(e12);

        T angle0 = std::acos(dot(e01, e02) / (len01 * len02));
        T angle1 = std::acos(dot(vec_scale(e01, -1.0), e12) / (len01 * len12));
        T angle2 = M_PI - angle0 - angle1;

        // Compute cotangents
        T cot0 = std::cos(angle0) / std::max(std::sin(angle0), 1e-10);
        T cot1 = std::cos(angle1) / std::max(std::sin(angle1), 1e-10);
        T cot2 = std::cos(angle2) / std::max(std::sin(angle2), 1e-10);

        // Update Laplacian for each vertex
        {
            auto diff1 = vec_sub(p1, p0);
            auto diff2 = vec_sub(p2, p0);
            auto laplacian = vec_scale(
                vec_add(vec_scale(diff1, cot2), vec_scale(diff2, cot1)), 0.5);
            vertex_data[v0].laplacian =
                vec_add(vertex_data[v0].laplacian, laplacian);
        }
        {
            auto diff0 = vec_sub(p0, p1);
            auto diff2 = vec_sub(p2, p1);
            auto laplacian = vec_scale(
                vec_add(vec_scale(diff0, cot2), vec_scale(diff2, cot0)), 0.5);
            vertex_data[v1].laplacian =
                vec_add(vertex_data[v1].laplacian, laplacian);
        }
        {
            auto diff0 = vec_sub(p0, p2);
            auto diff1 = vec_sub(p1, p2);
            auto laplacian = vec_scale(
                vec_add(vec_scale(diff0, cot1), vec_scale(diff1, cot0)), 0.5);
            vertex_data[v2].laplacian =
                vec_add(vertex_data[v2].laplacian, laplacian);
        }
    }

    // Compute final mean curvature
    for (uint32_t i = 0; i < num_vertices; ++i) {
        const auto &vdata = vertex_data[i];
        if (vdata.mixed_area > 1e-10) {
            // Mean curvature is half the magnitude of the Laplacian
            T mean_curvature =
                length(vdata.laplacian) / (2.0 * vdata.mixed_area);

            // For boundary vertices, scale by the number of neighbors
            if (vdata.neighbor_vertices.size() <
                6) { // Typical interior vertex has 6+ neighbors
                mean_curvature *=
                    6.0 / std::max<T>(vdata.neighbor_vertices.size(), 1);
            }

            result.setValue(i, mean_curvature);
        } else {
            result.setValue(i, 0.0);
        }
    }

    return result;
}

// Helper function to create a surface curvature operation
template <typename T>
auto make_surface_curvature(const Serie<uint32_t> &indices) {
    return [&indices](const Serie<T> &positions) {
        return surface_curvature(positions, indices);
    };
}

} // namespace geo
} // namespace df