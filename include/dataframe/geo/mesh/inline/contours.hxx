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

// Lookup tables from TypeScript implementation
const std::array<std::array<int, 2>, 8> lookupTable0 = {
    {{-1, -1}, {1, 2}, {0, 1}, {2, 0}, {2, 0}, {0, 1}, {1, 2}, {-1, -1}}};

template <size_t N>
inline Vector<N> interpolateVertex(const Vector<N> &v1, const Vector<N> &v2,
                                   double val1, double val2, double isoValue) {
    double t = (isoValue - val1) / (val2 - val1);
    return v1 + (v2 - v1) * t;
}

inline bool ok(double p0, double p1, double p2, double min, double max) {
    auto in = [](double p, double min, double max) {
        return p >= min && p <= max;
    };
    return in(p0, min, max) && in(p1, min, max) && in(p2, min, max);
}

} // namespace detail

// --------------------------------------------------------

template <size_t N>
inline std::ostream &operator<<(std::ostream &os, const IsoSegment<N> &seg) {
    auto displV = [&os](const Vector<N> &v) {
        os << "[";
        for (size_t i = 0; i < N - 1; ++i) {
            os << v[i] << ",";
        }
        os << v[v.size() - 1] << "]";
    };

    os << "IsoSegment(";
    displV(seg.p1);
    os << ", ";
    displV(seg.p2);
    os << "), value(" << seg.value << ")";
    return os;
}

template <size_t N>
inline Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                                     const std::string &attributeName,
                                     double isovalue) {
    const auto &values = mesh.template vertexAttribute<double>(attributeName);
    const auto &vertices = mesh.vertices();
    const auto &triangles = mesh.triangles();

    // Store triangle indices and their codes
    std::map<size_t, int> tri2code;
    std::map<int, std::map<int, std::vector<size_t>>> connectivity;

    // First pass: identify triangles crossed by isolines
    // TODO: optimization using kdtree or something else
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &tri = triangles[i];

        double p0 = values[tri[0]];
        double p1 = values[tri[1]];
        double p2 = values[tri[2]];

        // Skip if values are outside bounds
        double minVal = std::min({p0, p1, p2});
        double maxVal = std::max({p0, p1, p2});
        if (!detail::ok(p0, p1, p2, minVal, maxVal))
            continue;

        int t1 = (p0 >= isovalue) ? 1 : 0;
        int t2 = (p1 >= isovalue) ? 1 : 0;
        int t3 = (p2 >= isovalue) ? 1 : 0;

        int tri_code = t1 * 4 + t2 * 2 + t3;

        if (tri_code > 0 && tri_code < 7) {
            tri2code[i] = tri_code;
            std::array<int, 2> cut_edges = {detail::lookupTable0[tri_code][0],
                                            detail::lookupTable0[tri_code][1]};

            // Build connectivity
            for (int e = 0; e < 2; ++e) {
                int v0 = tri[cut_edges[e]];
                int v1 = tri[(cut_edges[e] + 1) % 3];

                int vmin = std::min(v0, v1);
                int vmax = std::max(v0, v1);

                connectivity[vmin][vmax].push_back(i);
            }
        }
    }

    Serie<IsoSegment<N>> segments;

    // Extract connected components
    while (!tri2code.empty()) {
        std::vector<Vector<N>> isoline;
        std::vector<double> values_t;

        size_t first_tri = tri2code.begin()->first;
        int code = tri2code[first_tri];
        tri2code.erase(first_tri);

        if (code < 1 || code > 6)
            continue;

        const auto &tri = triangles[first_tri];
        std::array<int, 2> cut_edges = {detail::lookupTable0[code][0],
                                        detail::lookupTable0[code][1]};

        // Process first triangle
        for (int e = 0; e < 2; ++e) {
            int v0 = tri[cut_edges[e]];
            int v1 = tri[(cut_edges[e] + 1) % 3];

            Vector<N> p = detail::interpolateVertex(
                vertices[v0], vertices[v1], values[v0], values[v1], isovalue);

            isoline.push_back(p);
        }

        // Connect segments
        segments.add({isoline[0], isoline[1], isovalue});
    }

    return segments;
}

template <size_t N>
inline Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                                     const std::string &attributeName,
                                     const std::vector<double> &isoValues) {
    Serie<IsoSegment<N>> allSegments;
    for (double isoValue : isoValues) {
        auto segments = contours(mesh, attributeName, isoValue);
        for (size_t i = 0; i < segments.size(); ++i) {
            allSegments.add(segments[i]);
        }
    }
    return allSegments;
}

template <size_t N>
Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                              const std::string &attributeName,
                              const Serie<double> &isoValues) {
    return contours(mesh, attributeName, isoValues.asArray());
}

} // namespace df
