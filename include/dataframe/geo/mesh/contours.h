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
#include <dataframe/geo/mesh/mesh.h>
#include <dataframe/geo/types.h>
#include <dataframe/types.h>
#include <stdexcept>

namespace df {

template <size_t N> struct IsoSegment {
    Vector<N> p1, p2;
    double value;
};

template <size_t N>
std::ostream &operator<<(std::ostream &os, const IsoSegment<N> &seg);

/**
 * @brief Compute iso-contours for a given iso-value on a triangulated mesh
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

/**
 * @brief Same as contours but using multiple iso values
 */
template <size_t N>
Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                              const std::string &attributeName,
                              const std::vector<double> &isoValues);

/**
 * @brief Same as contours but using multiple iso values
 */
template <size_t N>
Serie<IsoSegment<N>> contours(const Mesh<N> &mesh,
                              const std::string &attributeName,
                              const Serie<double> &isoValues);

// -----------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------

/**
 * @brief Generate iso-values given a min and max value and a spacing
 * @code
 * auto mesh = Mesh(positions, triangles);
 * mesh.addVertexAttribute("field", values);
 *
 * auto segments = marchingTriangles(mesh, "field", generateIsosBySpacing(0.0,
 * 0.7, 0.1));
 * @endcode
 */
Serie<double> generateIsosBySpacing(double min, double max, double spacing);

/**
 * @brief Generate iso-values given a min and max value and a number of isos
 * @code
 * auto mesh = Mesh(positions, triangles);
 * mesh.addVertexAttribute("field", values);
 *
 * auto segments = contours(mesh, "field", generateIsosByNumber(0.0, 0.7, 5));
 * @endcode
 */
Serie<double> generateIsosByNumber(double min, double max, size_t nbr = 10);

/**
 * @brief Generate iso-values given a min and max value and a number of iso
 * @code
 * auto mesh = Mesh(positions, triangles);
 * mesh.addVertexAttribute("field", values);
 *
 * // std::vector<double>
 * auto isos = generateIsos(0.0, 0.7, {0.2, 0.4, 0.6});
 * auto segments = contours(mesh, "field", isos);
 * @endcode
 *
 * @code
 * auto mesh = Mesh(positions, triangles);
 * mesh.addVertexAttribute("field", values);
 *
 * auto segments = contours(mesh, "field", generateIsos(0.0, 0.7, {}, true,
 * 0.15));
 * @endcode
 */
Serie<double>
generateIsos(double min, double max,
             const std::vector<double> &values = std::vector<double>(),
             bool useSpacing = false, double nbrOrSpacing = 10.0);

} // namespace df

#include "inline/contours.hxx"