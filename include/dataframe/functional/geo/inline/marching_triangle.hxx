namespace df {
namespace geo {

template <typename T>
bool MarchingTriangles<T>::setup(const GenSerie<uint32_t> &triangles,
                                 const std::array<T, 2> &bounds) {
    if (triangles.itemSize() != 3) {
        throw std::invalid_argument("Triangle serie must have itemSize=3");
    }

    isLocked_ = false;
    maxVertexIndex_ = 0;
    bounds_ = bounds;

    // Copy topology
    topology_ = triangles.clone();

    // Validate and find max vertex index
    for (uint32_t i = 0; i < topology_.count(); ++i) {
        auto tri = topology_.array(i);
        if (tri[0] == tri[1] || tri[0] == tri[2] || tri[1] == tri[2]) {
            throw std::invalid_argument(
                "Invalid triangle: duplicate vertex index");
        }
        maxVertexIndex_ = std::max({maxVertexIndex_, tri[0], tri[1], tri[2]});
    }

    isLocked_ = true;
    return true;
}

// Generate isolines for a given field and isovalue
template <typename T>
IsolineSet<T> MarchingTriangles<T>::isolines(const GenSerie<T> &field,
                                             T isovalue) {
    IsolineSet<T> result{
        GenSerie<uint32_t>(2, 0), // edges (itemSize=2 for vertex pairs)
        GenSerie<T>(1, 0) // values (itemSize=1 for interpolation values)
    };

    if (!isLocked_ || field.isEmpty()) {
        return result;
    }

    // If bounds not provided, compute them from field
    if (bounds_[0] == bounds_[1]) {
        T min = std::numeric_limits<T>::max();
        T max = std::numeric_limits<T>::lowest();
        for (uint32_t i = 0; i < field.count(); ++i) {
            T val = field.value(i);
            min = std::min(min, val);
            max = std::max(max, val);
        }
        bounds_ = {min, max};
    }

    // Maps to track triangle codes and connectivity
    std::map<size_t, int> tri2code;
    std::map<uint32_t, std::map<uint32_t, std::vector<size_t>>> connectivity;
    std::vector<std::pair<std::vector<uint32_t>, std::vector<T>>> isolines;

    // First pass: identify triangles crossed by isoline
    for (uint32_t i = 0; i < topology_.count(); ++i) {
        auto tri = topology_.array(i);

        T p0 = field.value(tri[0]);
        T p1 = field.value(tri[1]);
        T p2 = field.value(tri[2]);

        if (!checkValues(p0, p1, p2, bounds_[0], bounds_[1])) {
            continue;
        }

        int t1 = (p0 >= isovalue) ? 1 : 0;
        int t2 = (p1 >= isovalue) ? 1 : 0;
        int t3 = (p2 >= isovalue) ? 1 : 0;

        int triCode = lookupTable1[t1][t2][t3];

        if (triCode > 0 && triCode < 7) {
            tri2code[i] = triCode;
            auto cutEdges = lookupTable0[triCode];

            // Build connectivity information
            for (int e = 0; e < 2; ++e) {
                int v0 = cutEdges[e];
                int v1 = (v0 + 1) % 3;
                uint32_t vid0 = tri[v0];
                uint32_t vid1 = tri[v1];
                uint32_t vmin = std::min(vid0, vid1);
                uint32_t vmax = std::max(vid0, vid1);

                connectivity[vmin][vmax].push_back(i);
            }
        }
    }

    // Second pass: extract isolines
    while (!tri2code.empty()) {
        std::vector<uint32_t> isoline;
        std::vector<T> values;

        size_t currentTri = tri2code.begin()->first;
        int code = tri2code[currentTri];
        tri2code.erase(currentTri);

        if (code < 1 || code > 6) {
            continue;
        }

        auto tri = topology_.array(currentTri);
        auto cutEdges = lookupTable0[code];

        std::array<std::array<uint32_t, 2>, 2> edges;
        for (int e = 0; e < 2; ++e) {
            int v0 = cutEdges[e];
            int v1 = (v0 + 1) % 3;
            uint32_t vid0 = tri[v0];
            uint32_t vid1 = tri[v1];

            isoline.push_back(vid0);
            isoline.push_back(vid1);

            T t = (isovalue - field.value(vid0)) /
                  (field.value(vid1) - field.value(vid0));
            values.push_back(t);

            edges[e] = {std::min(vid0, vid1), std::max(vid0, vid1)};
        }

        auto nextEdge = edges[1];
        auto firstEdge = edges[0];
        size_t firstEdgeIndex = values.size();

        bool isClosed = false;
        int iterations = 0;
        const int MAX_ITERATIONS = 10000;

        while (iterations++ < MAX_ITERATIONS) {
            const auto &connectedTris = connectivity[nextEdge[0]][nextEdge[1]];
            if (connectedTris.size() <= 1)
                break;

            size_t nextTri = (connectedTris[0] == currentTri)
                                 ? connectedTris[1]
                                 : connectedTris[0];

            auto triIter = tri2code.find(nextTri);
            if (triIter == tri2code.end())
                break;

            code = triIter->second;
            tri2code.erase(triIter);
            currentTri = nextTri;

            auto nextTriVerts = topology_.array(nextTri);
            cutEdges = lookupTable0[code];

            for (int e = 0; e < 2; ++e) {
                int v0 = cutEdges[e];
                int v1 = (v0 + 1) % 3;
                uint32_t vid0 = nextTriVerts[v0];
                uint32_t vid1 = nextTriVerts[v1];
                uint32_t vmin = std::min(vid0, vid1);
                uint32_t vmax = std::max(vid0, vid1);

                if (vmin != nextEdge[0] || vmax != nextEdge[1]) {
                    isoline.push_back(vid0);
                    isoline.push_back(vid1);

                    T t = (isovalue - field.value(vid0)) /
                          (field.value(vid1) - field.value(vid0));
                    values.push_back(t);

                    nextEdge = {vmin, vmax};
                    break;
                }
            }

            if (nextEdge[0] == firstEdge[0] && nextEdge[1] == firstEdge[1]) {
                isoline.push_back(isoline[0]);
                isoline.push_back(isoline[1]);
                values.push_back(values[0]);
                isClosed = true;
                break;
            }
        }

        if (iterations >= MAX_ITERATIONS)
            continue;

        if (isClosed) {
            isolines.emplace_back(isoline, values);
        } else {
            std::vector<uint32_t> connectedEdges;
            std::vector<T> connectedValues;

            for (size_t j = isoline.size() - 1; j >= 2 * firstEdgeIndex; --j) {
                connectedEdges.push_back(isoline[j]);
            }
            for (size_t j = values.size() - 1; j >= firstEdgeIndex; --j) {
                connectedValues.push_back(1.0 - values[j]);
            }

            for (size_t j = 0; j < 2 * firstEdgeIndex; ++j) {
                connectedEdges.push_back(isoline[j]);
            }
            for (size_t j = 0; j < firstEdgeIndex; ++j) {
                connectedValues.push_back(values[j]);
            }

            isolines.emplace_back(connectedEdges, connectedValues);
        }
    }

    // Convert collected isolines to GenSerie format
    uint32_t totalEdges = 0;
    for (const auto &isoline : isolines) {
        totalEdges += isoline.first.size() / 2;
    }

    result.edges = GenSerie<uint32_t>(2, totalEdges);
    result.values = GenSerie<T>(1, totalEdges);

    uint32_t offset = 0;
    for (const auto &isoline : isolines) {
        for (size_t i = 0; i < isoline.first.size(); i += 2) {
            result.edges.setArray(offset,
                                  {isoline.first[i], isoline.first[i + 1]});
            result.values.setValue(offset, isoline.second[i / 2]);
            ++offset;
        }
    }

    return result;
}

template <typename T>
IsoContours<T> MarchingTriangles<T>::computeContourCoordinates(
    const IsolineSet<T> &isolines, const GenSerie<T> &vertices) const {
    if (vertices.itemSize() != 3) {
        throw std::invalid_argument(
            "Vertices must have itemSize=3 (3D points)");
    }

    IsoContours<T> contours;
    contours.points = GenSerie<T>(3, isolines.edges.count());
    contours.segments = GenSerie<uint32_t>(2, isolines.edges.count());

    uint32_t pointIndex = 0;

    for (uint32_t i = 0; i < isolines.edges.count(); ++i) {
        // Get edge vertices indices
        auto edge = isolines.edges.array(i);
        T t = isolines.values.value(i);

        // Get vertex positions
        auto v1 = vertices.array(edge[0]);
        auto v2 = vertices.array(edge[1]);

        // Interpolate position
        std::vector<T> pos(3);
        for (int j = 0; j < 3; ++j) {
            pos[j] = v1[j] + t * (v2[j] - v1[j]);
        }

        // Store interpolated point
        contours.points.setArray(pointIndex, pos);

        // Create segment if not first point
        if (i > 0 && i % 2 == 1) {
            contours.segments.setArray((i - 1) / 2,
                                       {pointIndex - 1, pointIndex});
        }

        ++pointIndex;
    }

    return contours;
}

// Convenience method to directly get contour coordinates
template <typename T>
IsoContours<T> MarchingTriangles<T>::isocontours(
    const GenSerie<T> &field, const GenSerie<T> &vertices, T isovalue) {
    auto isolines = this->isolines(field, isovalue);
    return computeContourCoordinates(isolines, vertices);
}

template <typename T>
bool MarchingTriangles<T>::inRange(T p, T min, T max) const {
    return p >= min && p <= max;
}

template <typename T>
bool MarchingTriangles<T>::checkValues(T p0, T p1, T p2, T min, T max) const {
    return inRange(p0, min, max) && inRange(p1, min, max) &&
           inRange(p2, min, max);
}

// Initialize lookup tables
template <typename T>
const std::array<std::array<int, 2>, 8> MarchingTriangles<T>::lookupTable0 = {
    std::array<int, 2>{{-1, -1}}, std::array<int, 2>{{1, 2}},
    std::array<int, 2>{{0, 1}},   std::array<int, 2>{{2, 0}},
    std::array<int, 2>{{2, 0}},   std::array<int, 2>{{0, 1}},
    std::array<int, 2>{{1, 2}},   std::array<int, 2>{{-1, -1}}};

template <typename T>
const std::array<std::array<std::array<int, 2>, 2>, 2>
    MarchingTriangles<T>::lookupTable1 = {
        {{{{0, 1}, {2, 3}}}, {{{4, 5}, {6, 7}}}}};

} // namespace geo
} // namespace df