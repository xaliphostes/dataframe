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

namespace df {

namespace detail {

// Compute area of a triangle using cross product method
template <typename T>
T triangle_area(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
    // Get two edge vectors
    T e1x = v2[0] - v1[0];
    T e1y = v2[1] - v1[1];
    T e1z = v2[2] - v1[2];

    T e2x = v3[0] - v1[0];
    T e2y = v3[1] - v1[1];
    T e2z = v3[2] - v1[2];

    // Cross product
    T nx = e1y * e2z - e1z * e2y;
    T ny = e1z * e2x - e1x * e2z;
    T nz = e1x * e2y - e1y * e2x;

    // Area is half the length of the cross product
    return T{0.5} * std::sqrt(nx * nx + ny * ny + nz * nz);
}

// Compute area of a triangle in 2D using cross product z-component
template <typename T>
T triangle_area_2d(const Vector2 &v1, const Vector2 &v2, const Vector2 &v3) {
    // Compute cross product z-component of two edge vectors
    T e1x = v2[0] - v1[0];
    T e1y = v2[1] - v1[1];
    T e2x = v3[0] - v1[0];
    T e2y = v3[1] - v1[1];

    return T{0.5} * std::abs(e1x * e2y - e1y * e2x);
}
} // namespace detail

template <typename T>
Serie<T> area(const Serie<Vector3> &vertices,
              const Serie<std::array<int, 3>> &triangles) {
    if (vertices.empty() || triangles.empty()) {
        return Serie<T>();
    }

    return triangles.map([&vertices](const auto &triangle, size_t) {
        const auto &v1 = vertices[triangle[0]];
        const auto &v2 = vertices[triangle[1]];
        const auto &v3 = vertices[triangle[2]];
        return detail::triangle_area<T>(v1, v2, v3);
    });
}

/**
 * Compute areas of a series of triangles in 2D
 * @param vertices Serie of 2D vertices
 * @param triangles Serie of index triplets defining triangles
 * @return Serie of triangle areas
 */
template <typename T>
Serie<T> area(const Serie<Vector2> &vertices,
              const Serie<std::array<int, 3>> &triangles) {
    if (vertices.empty() || triangles.empty()) {
        return Serie<T>();
    }

    return triangles.map([&vertices](const auto &triangle, size_t) {
        const auto &v1 = vertices[triangle[0]];
        const auto &v2 = vertices[triangle[1]];
        const auto &v3 = vertices[triangle[2]];
        return detail::triangle_area_2d<T>(v1, v2, v3);
    });
}

// Binding functions for pipeline operations
template <typename T>
auto bind_area(const Serie<std::array<int, 3>> &triangles) {
    return [&triangles](const auto &vertices) {
        return area<T>(vertices, triangles);
    };
}

} // namespace df