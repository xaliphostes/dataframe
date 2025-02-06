
namespace df {

template <size_t N>
Mesh<N>::Mesh(const Serie<VertexType> &vertices, const Triangles &triangles)
    : vertices_(vertices), triangles_(triangles) {
    if (!isValid()) {
        throw std::invalid_argument(
            "Invalid mesh construction: vertices or triangles are invalid (empty?)");
    }
}

template <size_t N>
template <typename T>
void Mesh<N>::addVertexAttribute(const std::string &name,
                                 const Serie<T> &values) {
    validateAttributeSize(name, values.size(), true);
    vertex_attributes_.add(name, values);
}

template <size_t N>
template <typename T>
void Mesh<N>::addTriangleAttribute(const std::string &name,
                                   const Serie<T> &values) {
    validateAttributeSize(name, values.size(), false);
    triangle_attributes_.add(name, values);
}

template <size_t N>
void Mesh<N>::removeVertexAttribute(const std::string &name) {
    vertex_attributes_.remove(name);
}

template <size_t N>
void Mesh<N>::removeTriangleAttribute(const std::string &name) {
    triangle_attributes_.remove(name);
}

template <size_t N>
bool Mesh<N>::hasVertexAttribute(const std::string &name) const {
    return vertex_attributes_.has(name);
}

template <size_t N>
bool Mesh<N>::hasTriangleAttribute(const std::string &name) const {
    return triangle_attributes_.has(name);
}

template <size_t N>
template <typename T>
const Serie<T> &Mesh<N>::vertexAttribute(const std::string &name) const {
    return vertex_attributes_.get<T>(name);
}

template <size_t N>
template <typename T>
const Serie<T> &Mesh<N>::triangleAttribute(const std::string &name) const {
    return triangle_attributes_.get<T>(name);
}

template <size_t N>
void Mesh<N>::validateAttributeSize(const std::string &name, size_t size,
                                    bool isVertex) const {
    if (name.empty()) {
        throw std::invalid_argument("Attribute name cannot be empty");
    }

    size_t expected_size = isVertex ? vertexCount() : triangleCount();
    if (size != expected_size) {
        throw std::invalid_argument("Attribute size mismatch: expected " +
                                    std::to_string(expected_size) +
                                    " values, got " + std::to_string(size));
    }
}

template <size_t N> bool Mesh<N>::isValid() const {
    if (vertices_.empty() || triangles_.empty()) {
        return false;
    }

    // Check triangle indices are within bounds
    for (size_t i = 0; i < triangles_.size(); ++i) {
        const auto &tri = triangles_[i];
        if (tri[0] >= vertices_.size() || tri[1] >= vertices_.size() ||
            tri[2] >= vertices_.size()) {
            return false;
        }
    }

    return true;
}

} // namespace df