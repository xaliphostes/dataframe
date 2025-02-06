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

#pragma once
#include <array>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/mesh.h>
#include <dataframe/geo/types.h>
#include <dataframe/types.h>
#include <stdexcept>

namespace df {

template <size_t N> struct IsoSegment {
    Vector<N> p1, p2;
    double value;
};

// template <size_t N>
// std::ostream &operator<<(std::ostream &os, const IsoSegment<N> &seg) {
//     // os << seg.p1 << " " << seg.p2 << " :" << seg.value << seg.value;
//     os << Vector<N>(seg.p1) << " " << Vector<N>(seg.p2) << " :" << seg.value
//     << std::endl; return os;
// }

template <size_t N>
std::ostream &operator<<(std::ostream &os, const IsoSegment<N> &seg) {
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

/**
 * @brief Compute contour-contours for a given iso-value on a triangulated mesh
 * (2D or 3D)
 *
 * @param mesh Mesh object
 * @param attributeName Name of the attribute to use for contouring
 * @param isoValue Iso-value
 * @return Serie of IsoSegments
 *
 * @code
 * #include <dataframe/geo/mesh.h>
 * #include <dataframe/geo/contours.h>
 *
 * auto mesh = Mesh(positions, triangles);
 *
 * auto isoSegments = contours(mesh, "temperature", 25.0);
 *
 * isoSegments.each([](const IsoSegment<3> &seg, size_t) {
 *    std::cout << seg.p1 << " " << seg.p2 << std::endl;
 * });
 * @endcode
 */
template <size_t N>
Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                              const std::string &attributeName,
                              double isoValue);

template <size_t N>
Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                              const std::string &attributeName,
                              const std::vector<double> &isoValues);

} // namespace df

#include "inline/contours.hxx"