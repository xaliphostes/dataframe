/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>
#include <stdexcept>

namespace df {

/**
 * @brief Class representing a triangulated surface mesh in 2D or 3D
 * @tparam N Dimension of the mesh (2 or 3)
 * @code
 * // Create a 2D mesh
 * Mesh2D mesh({{0,0}, {1,0}, {0,1}}, {{0,1,2}});
 *
 * // Add vertex attribute
 * mesh.addVertexAttribute("temperature", Serie<double>{20.0, 25.0, 22.0});
 *
 * // Add vector attributes
 * mesh.addVertexAttribute("vec", Serie<Vector2>{{0,1}, {1,0}, {-1,-1}});
 * mesh.addVertexAttribute("normals", df::normals(mesh.vertices(),
 * mesh.triangles()));
 *
 * // Access attributes
 * const auto& temp  = mesh.vertexAttribute<double>("temperature");
 * const auto& norms = mesh.vertexAttribute<Vector2>("normals");
 * const auto& vec2  = mesh.vertexAttribute<Vector2>("vec2");
 *
 * @endcode
 */
template <size_t N> class Mesh {
  public:
    static_assert(N == 2 || N == 3, "Mesh dimension must be 2 or 3");

    using VertexType = Vector<N>;

    Mesh() = default;

    /**
     * @brief Construct a mesh from vertices and triangles
     * @param vertices Vertex positions
     * @param triangles Triangle indices
     */
    Mesh(const Serie<VertexType> &vertices, const Triangles &triangles);

    // Basic accessors
    size_t vertexCount() const { return vertices_.size(); }
    size_t triangleCount() const { return triangles_.size(); }
    bool empty() const { return vertices_.empty() || triangles_.empty(); }

    // Geometry access
    const Serie<VertexType> &vertices() const { return vertices_; }
    const Triangles &triangles() const { return triangles_; }
    Serie<VertexType> &vertices() { return vertices_; }
    Triangles &triangles() { return triangles_; }

    // Dataframe access
    const Dataframe &vertexAttributes() const { return vertex_attributes_; }
    Dataframe &vertexAttributes() { return vertex_attributes_; }
    const Dataframe &triangleAttributes() const { return triangle_attributes_; }
    Dataframe &triangleAttributes() { return triangle_attributes_; }

    // Attribute convenience methods
    template <typename T>
    void addVertexAttribute(const std::string &name, const Serie<T> &values);
    template <typename T>
    void addTriangleAttribute(const std::string &name, const Serie<T> &values);

    bool hasVertexAttribute(const std::string &name) const;
    bool hasTriangleAttribute(const std::string &name) const;

    template <typename T>
    const Serie<T> &vertexAttribute(const std::string &name) const;
    template <typename T>
    const Serie<T> &triangleAttribute(const std::string &name) const;

    void removeVertexAttribute(const std::string &name);
    void removeTriangleAttribute(const std::string &name);

    // Validation
    bool isValid() const;

  private:
    Serie<VertexType> vertices_;
    Triangles triangles_;
    Dataframe vertex_attributes_;
    Dataframe triangle_attributes_;

    void validateAttributeSize(const std::string &name, size_t size,
                               bool isVertex) const;
};

// Type aliases for common dimensions
using Mesh2D = Mesh<2>;
using Mesh3D = Mesh<3>;

} // namespace df

#include "inline/mesh.hxx"