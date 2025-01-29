#pragma once
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>
#include <map>

namespace df {
namespace geo {

/**
 * A simple mesh class that stores vertex positions, element indices, as well as
 * vertex and element attributes. An element is simply the combel with the
 * highest dimension. That is to say, a triangle is an element of a triangular
 * mesh, and a tetrahedron is an element of a tetrahedral mesh. In this class,
 * hybrid meshes are not supported (yet).
 */
class Mesh {
  public:
    using Vertex = Array<double>;
    using Element = Array<uint32_t>;

    Mesh() = default;
    Mesh(const Indices &indices, const Positions &positions);

    void addVertexAttribute(const std::string &name, const Attribute &values);
    void addElementAttribute(const std::string &name, const Attribute &values);

    void print(std::ostream &out = std::cout) const;

    const Positions& vertices() const;
    const Indices& indices() const;
    const Attributes& vertexAttributes() const;
    const Attributes& elementAttributes() const;

  private:
    Indices indices_;
    Positions vertices_;
    Attributes vattributes_;
    Attributes eattributes_;
};

} // namespace geo
} // namespace df

#include "inline/Mesh.hxx"
