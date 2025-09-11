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
        inline double triangle_area_3d(const Vector3& v1, const Vector3& v2, const Vector3& v3)
        {
            // Get two edge vectors
            double e1x = v2[0] - v1[0];
            double e1y = v2[1] - v1[1];
            double e1z = v2[2] - v1[2];

            double e2x = v3[0] - v1[0];
            double e2y = v3[1] - v1[1];
            double e2z = v3[2] - v1[2];

            // Cross product
            double nx = e1y * e2z - e1z * e2y;
            double ny = e1z * e2x - e1x * e2z;
            double nz = e1x * e2y - e1y * e2x;

            // Area is half the length of the cross product
            return 0.5 * std::sqrt(nx * nx + ny * ny + nz * nz);
        }

        // Compute area of a triangle in 2D using cross product z-component
        inline double triangle_area_2d(const Vector2& v1, const Vector2& v2, const Vector2& v3)
        {
            // Compute cross product z-component of two edge vectors
            double e1x = v2[0] - v1[0];
            double e1y = v2[1] - v1[1];
            double e2x = v3[0] - v1[0];
            double e2y = v3[1] - v1[1];

            return 0.5 * std::abs(e1x * e2y - e1y * e2x);
        }
    } // namespace detail

    // Overload for Vector2
    template <>
    inline Serie<double> area<Vector2>(const Serie<Vector2>& vertices, const Triangles& triangles)
    {
        if (vertices.empty() || triangles.empty()) {
            return Serie<double>();
        }

        return triangles.map([&vertices](const auto& triangle, size_t) {
            const auto& v1 = vertices[triangle[0]];
            const auto& v2 = vertices[triangle[1]];
            const auto& v3 = vertices[triangle[2]];
            return detail::triangle_area_2d(v1, v2, v3);
        });
    }

    // Overload for Vector3
    template <>
    inline Serie<double> area<Vector3>(const Serie<Vector3>& vertices, const Triangles& triangles)
    {
        if (vertices.empty() || triangles.empty()) {
            return Serie<double>();
        }

        return triangles.map([&vertices](const auto& triangle, size_t) {
            const auto& v1 = vertices[triangle[0]];
            const auto& v2 = vertices[triangle[1]];
            const auto& v3 = vertices[triangle[2]];
            return detail::triangle_area_3d(v1, v2, v3);
        });
    }

    // Binding functions for pipeline operations
    template <> inline auto bind_area<Vector2>(const Triangles& triangles)
    {
        return [&triangles](
                   const Serie<Vector2>& vertices) { return area<Vector2>(vertices, triangles); };
    }

    template <> inline auto bind_area<Vector3>(const Triangles& triangles)
    {
        return [&triangles](
                   const Serie<Vector3>& vertices) { return area<Vector3>(vertices, triangles); };
    }

} // namespace df