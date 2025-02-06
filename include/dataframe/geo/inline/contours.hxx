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

// Lookup table for marching triangles cases
const std::array<std::array<int, 4>, 8> TRIANGLE_TABLE = {{
    {-1, -1, -1, -1}, // Case 0: no intersection
    {0, 1, -1, -1},   // Case 1: intersection on edges 0-1
    {1, 2, -1, -1},   // Case 2: intersection on edges 1-2
    {0, 2, -1, -1},   // Case 3: intersection on edges 0-2
    {0, 1, 1, 2},     // Case 4: intersections on edges 0-1 and 1-2
    {0, 2, 1, 2},     // Case 5: intersections on edges 0-2 and 1-2
    {0, 1, 0, 2},     // Case 6: intersections on edges 0-1 and 0-2
    {-1, -1, -1, -1}  // Case 7: no intersection
}};

template <size_t N>
Vector<N> interpolateVertex(const Vector<N> &v1, const Vector<N> &v2,
                            double val1, double val2, double isoValue) {
    double t = (isoValue - val1) / (val2 - val1);
    return v1 + (v2 - v1) * t;
}

template <size_t N>
inline Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                                     const std::string &attributeName,
                                     double isoValue) {
    const auto &values = mesh.template vertexAttribute<double>(attributeName);
    const auto &vertices = mesh.vertices();
    const auto &triangles = mesh.triangles();

    Serie<IsoSegment<N>> segments;

    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &tri = triangles[i];

        // Determine case index based on vertex values
        int caseIndex = 0;
        std::array<bool, 3> vertexStates;
        for (int j = 0; j < 3; ++j) {
            vertexStates[j] = values[tri[j]] >= isoValue;
            caseIndex |= (vertexStates[j] ? 1 : 0) << j;
        }

        const auto &edges = TRIANGLE_TABLE[caseIndex];
        if (edges[0] == -1)
            continue;

        // Process edge intersections
        for (int j = 0; j < 4; j += 2) {
            if (edges[j] == -1)
                break;

            int edge1 = edges[j];
            int edge2 = edges[j + 1];

            int v1 = tri[edge1];
            int v2 = tri[(edge1 + 1) % 3];
            int v3 = tri[edge2];
            int v4 = tri[(edge2 + 1) % 3];

            Vector<N> p1 = interpolateVertex(vertices[v1], vertices[v2],
                                             values[v1], values[v2], isoValue);

            Vector<N> p2 = interpolateVertex(vertices[v3], vertices[v4],
                                             values[v3], values[v4], isoValue);

            segments.add({p1, p2, isoValue});
        }
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

} // namespace df
