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

#include "../from_dims.h"

namespace df {
namespace grid {
namespace cartesian {

template <size_t N>
Serie<Vector<N>> from_points(const iVector<N> &npts,
                             const std::vector<double> &p1,
                             const std::vector<double> &p2) {
    // Validate input
    if (p1.size() != N || p2.size() != N) {
        throw std::invalid_argument("Input points must have the same dimension "
                                    "as template parameter N");
    }

    // Convert input points to Vector<N>
    Vector<N> min_corner, max_corner;
    for (size_t i = 0; i < N; ++i) {
        min_corner[i] = std::min(p1[i], p2[i]);
        max_corner[i] = std::max(p1[i], p2[i]);
    }

    // Calculate dimensions and center
    Vector<N> dimensions, center;
    for (size_t i = 0; i < N; ++i) {
        dimensions[i] = max_corner[i] - min_corner[i];
        center[i] = min_corner[i] + dimensions[i] / 2.0;
    }

    // Use from_dims to generate the grid
    return from_dims(npts, center, dimensions);
}

} // namespace cartesian
} // namespace grid
} // namespace df
