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
Serie<double> generateIsosBySpacing(double min, double max, double spacing) {
    std::vector<double> isos;

    if (max < min) {
        throw std::runtime_error("Min should be less than max");
    }

    if (std::abs(max - min) / spacing > 500) {
        spacing = std::abs(max - min) / 500;
        std::cerr << "WARNING: increasing the spacing to " << spacing
                  << " to avoid too many isos" << std::endl;
    }

    if (min < 0 && max > 0) {
        // Handle range crossing zero
        double value = spacing;
        while (value >= min + spacing) {
            isos.push_back(value);
            value -= spacing;
        }
        value = 0;
        while (value <= max - spacing) {
            isos.push_back(value);
            value += spacing;
        }
    } else {
        // Handle range not crossing zero
        double scale = 1;
        if (max < 0) {
            scale = -1;
            std::swap(min, max);
        }

        if (min * scale >= max * scale)
            return {};

        int valueInc = static_cast<int>(min * scale / spacing);
        if (valueInc * spacing < min * scale)
            valueInc++;

        double value = valueInc * spacing;
        while (value <= max * scale) {
            isos.push_back(value * scale);
            value += spacing;
        }
    }

    std::sort(isos.begin(), isos.end());
    return Serie(isos);
}

/**
 * @brief Generate iso-values given a min and max value and a number of isos
 * @code
 * auto mesh = Mesh(positions, triangles);
 * mesh.addVertexAttribute("field", values);
 *
 * auto segments = contours(mesh, "field", generateIsosByNumber(0.0, 0.7, 5));
 * @endcode
 */
Serie<double> generateIsosByNumber(double min, double max, size_t nbr = 10) {
    if (min >= max)
        return {};
    double epsilon = (max - min) / nbr;
    return generateIsosBySpacing(min, max, epsilon);
}

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
             bool useSpacing = false, double nbrOrSpacing = 10.0) {
    if (!values.empty()) {
        std::vector<double> filtered;
        std::copy_if(values.begin(), values.end(), std::back_inserter(filtered),
                     [min, max](double v) { return v >= min && v <= max; });
        return filtered;
    }

    if (useSpacing) {
        return generateIsosBySpacing(min, max, nbrOrSpacing);
    }
    return generateIsosByNumber(min, max, static_cast<size_t>(nbrOrSpacing));
}

} // namespace df

#include "inline/contours.hxx"