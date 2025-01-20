namespace df {
namespace geo {

inline Mesh::Mesh(const Indices &indices, const Positions &positions)
    : indices_(indices), vertices_(positions) {}

inline void Mesh::addVertexAttribute(const std::string &name,
                                     const Attribute &values) {
    if (values.count() != vertices_.count()) {
        throw std::invalid_argument("Attribute named " + name + " with count " +
                                    std::to_string(values.count()) +
                                    " must match vertex count (" +
                                    std::to_string(vertices_.count()) + ")");
    }
    vattributes_[name] = values;
}

inline void Mesh::addElementAttribute(const std::string &name,
                                      const Attribute &values) {
    if (values.count() != indices_.count()) {
        throw std::invalid_argument("Attribute named " + name + " with count " +
                                    std::to_string(values.count()) +
                                    " must match element count (" +
                                    std::to_string(indices_.count()) + ")");
    }
    eattributes_[name] = values;
}

inline void Mesh::print(std::ostream &out) const {
    out << "Mesh with " << vertices_.count() << " vertices and "
        << indices_.count() << " elements" << std::endl;
    out << "Vertex attributes: " << std::endl;
    if (vattributes_.empty()) {
        out << "  (none)" << std::endl;
    } else {
        for (const auto &attr : vattributes_)
            out << "  " << attr.first << ": " << attr.second.count()
                << " values" << std::endl;
    }
    out << "Element attributes: " << std::endl;
    if (eattributes_.empty()) {
        out << "  (none)" << std::endl;
    } else {
        for (const auto &attr : eattributes_) {
            out << "  " << attr.first << ": " << attr.second.count()
                << " values" << std::endl;
        }
    }
}

inline const Positions &Mesh::vertices() const { return vertices_; }

const Indices &Mesh::indices() const { return this->indices_; }

const Attributes &Mesh::vertexAttributes() const { return vattributes_; }

const Attributes &Mesh::elementAttributes() const { return eattributes_; }

} // namespace geo
} // namespace df
