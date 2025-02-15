
namespace df {

template <size_t N>
inline Mesh<N>::Mesh(const Serie<VertexType> &vertices,
                     const Triangles &triangles)
    : vertices_(vertices), triangles_(triangles) {
    if (!isValid()) {
        throw std::invalid_argument("Invalid mesh construction: vertices or "
                                    "triangles are invalid (empty?)");
    }
    buildTopology();
}

template <size_t N>
template <typename T>
inline void Mesh<N>::addVertexAttribute(const std::string &name,
                                        const Serie<T> &values) {
    validateAttributeSize(name, values.size(), true);
    vertex_attributes_.add(name, values);
}

template <size_t N>
template <typename T>
inline void Mesh<N>::addTriangleAttribute(const std::string &name,
                                          const Serie<T> &values) {
    validateAttributeSize(name, values.size(), false);
    triangle_attributes_.add(name, values);
}

template <size_t N>
inline void Mesh<N>::removeVertexAttribute(const std::string &name) {
    vertex_attributes_.remove(name);
}

template <size_t N>
inline void Mesh<N>::removeTriangleAttribute(const std::string &name) {
    triangle_attributes_.remove(name);
}

template <size_t N>
inline bool Mesh<N>::hasVertexAttribute(const std::string &name) const {
    return vertex_attributes_.has(name);
}

template <size_t N>
inline bool Mesh<N>::hasTriangleAttribute(const std::string &name) const {
    return triangle_attributes_.has(name);
}

template <size_t N>
template <typename T>
inline const Serie<T> &Mesh<N>::vertexAttribute(const std::string &name) const {
    return vertex_attributes_.get<T>(name);
}

template <size_t N>
template <typename T>
inline const Serie<T> &
Mesh<N>::triangleAttribute(const std::string &name) const {
    return triangle_attributes_.get<T>(name);
}

template <size_t N>
inline void Mesh<N>::validateAttributeSize(const std::string &name, size_t size,
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

template <size_t N> inline bool Mesh<N>::isValid() const {
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

template <size_t N> inline void Mesh<N>::buildTopology() {
    // Initialize adjacency list
    adjacency_.resize(vertices_.size());

    // Build adjacency from triangles
    for (size_t i = 0; i < triangles_.size(); ++i) {
        const auto &tri = triangles_[i];
        for (size_t j = 0; j < 3; ++j) {
            size_t v1 = static_cast<size_t>(tri[j]);
            size_t v2 = static_cast<size_t>(tri[(j + 1) % 3]);
            adjacency_[v1].push_back(v2);
            adjacency_[v2].push_back(v1);
        }
    }

    // Find border nodes
    findBorderNodes();
}

template <size_t N> inline void Mesh<N>::findBorderNodes() {
    std::vector<std::vector<size_t>> edge_count(vertices_.size());

    // Count edges
    for (size_t i = 0; i < triangles_.size(); ++i) {
        const auto &tri = triangles_[i];
        for (size_t j = 0; j < 3; ++j) {
            size_t v1 = static_cast<size_t>(tri[j]);
            size_t v2 = static_cast<size_t>(tri[(j + 1) % 3]);
            edge_count[v1].push_back(v2);
        }
    }

    // Nodes with non-manifold edges are border nodes
    for (size_t i = 0; i < edge_count.size(); ++i) {
        std::sort(edge_count[i].begin(), edge_count[i].end());
        auto it = std::unique(edge_count[i].begin(), edge_count[i].end());
        if (it != edge_count[i].end()) {
            border_nodes_.push_back(i);
        }
    }
}

template <size_t N>
inline const std::vector<size_t> &Mesh<N>::neighbors(size_t node_idx) const {
    return adjacency_[node_idx];
}

template <size_t N>
inline const std::vector<size_t> &Mesh<N>::borderNodes() const {
    return border_nodes_;
}

} // namespace df