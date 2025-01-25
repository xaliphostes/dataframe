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

#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <vector>

namespace df {
namespace geo {

// ------------------------------------------------

template <typename T> struct IsolineSet {
    GenSerie<uint32_t> edges; // Edge vertex indices (itemSize=2)
    GenSerie<T> values;       // Interpolation values (itemSize=1)
};

// ------------------------------------------------

template <typename T> struct IsoContours {
    GenSerie<T> points; // Contour points coordinates (itemSize=3 for 3D points)
    GenSerie<uint32_t> segments; // Segments connectivity (itemSize=2)
};

// ------------------------------------------------

template <typename T> class MarchingTriangles {
  public:
    bool setup(const GenSerie<uint32_t> &, const std::array<T, 2> & = {});

    IsolineSet<T> isolines(const GenSerie<T> &, T);
    IsoContours<T> isocontours(const GenSerie<T> &, const GenSerie<T> &, T);

  private:
    // Lookup tables
    static const std::array<std::array<int, 2>, 8> lookupTable0;
    static const std::array<std::array<std::array<int, 2>, 2>, 2> lookupTable1;
    GenSerie<uint32_t> topology_; // Triangle topology (itemSize=3)
    bool isLocked_ = false;
    uint32_t maxVertexIndex_ = 0;
    std::array<T, 2> bounds_;
    bool inRange(T p, T min, T max) const;
    bool checkValues(T p0, T p1, T p2, T min, T max) const;
    IsoContours<T> computeContourCoordinates(const IsolineSet<T> &isolines,
                                             const GenSerie<T> &vertices) const;
};

// ------------------------------------------------

} // namespace geo
} // namespace df

#include "inline/marching_triangle.hxx"