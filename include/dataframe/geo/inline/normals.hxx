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

// Compute normal vector to a line segment in 2D
template <typename T>
Vector2 segment_normal(const Vector2 &v1, const Vector2 &v2) {
    // Get segment direction vector
    T dx = v2[0] - v1[0];
    T dy = v2[1] - v1[1];

    // Perpendicular vector (-dy, dx) normalized
    T length = std::sqrt(dx * dx + dy * dy);
    if (length < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Zero-length segment encountered");
    }

    return {-dy / length, dx / length};
}

// Compute normal vector to a triangle in 3D using cross product
template <typename T>
Vector3 triangle_normal(const Vector3 &v1, const Vector3 &v2,
                        const Vector3 &v3) {
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

    // Normalize
    T length = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (length < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Degenerate triangle encountered");
    }

    return {nx / length, ny / length, nz / length};
}
} // namespace detail

/**
 * Compute normals for a series of line segments (2D)
 * @param vertices Serie of 2D vertices
 * @param segments Serie of index pairs defining segments
 * @return Serie of normalized normal vectors for each segment
 */
template <typename T>
Serie<Vector2> normals(const Serie<Vector2> &vertices,
                       const Serie<iVector2> &segments) {
    if (vertices.empty() || segments.empty()) {
        return Serie<Vector2>();
    }

    return segments.map([&vertices](const auto &segment, size_t) {
        // Get vertices for this segment
        const auto &v1 = vertices[segment[0]];
        const auto &v2 = vertices[segment[1]];

        return detail::segment_normal<T>(v1, v2);
    });
}

/**
 * Compute normals for a series of triangles (3D)
 * @param vertices Serie of 3D vertices
 * @param triangles Serie of index triplets defining triangles
 * @return Serie of normalized normal vectors for each triangle
 */
template <typename T>
Serie<Vector3> normals(const Serie<Vector3> &vertices,
                       const Serie<iVector3> &triangles) {
    if (vertices.empty() || triangles.empty()) {
        return Serie<Vector3>();
    }

    return triangles.map([&vertices](const auto &triangle, size_t) {
        // Get vertices for this triangle
        const auto &v1 = vertices[triangle[0]];
        const auto &v2 = vertices[triangle[1]];
        const auto &v3 = vertices[triangle[2]];

        return detail::triangle_normal<T>(v1, v2, v3);
    });
}

// Binding functions for pipeline operations
template <typename T> auto bind_normals(const Serie<iVector2> &segments) {
    return [&segments](const Serie<Vector2> &vertices) {
        return normals<T>(vertices, segments);
    };
}

template <typename T> auto bind_normals(const Serie<iVector3> &triangles) {
    return [&triangles](const Serie<Vector3> &vertices) {
        return normals<T>(vertices, triangles);
    };
}

} // namespace df