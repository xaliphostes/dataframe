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
namespace grid {
namespace cartesian {

template <size_t N>
Serie<Vector<N>> from_dims(const iVector<N> &npts, const Vector<N> &center,
                           const Vector<N> &dimensions) {
    // Validate input
    for (size_t i = 0; i < N; ++i) {
        if (npts[i] < 1) {
            throw std::invalid_argument(
                "Number of points must be at least 1 in each dimension");
        }
        if (dimensions[i] <= 0) {
            throw std::invalid_argument("Dimensions must be positive");
        }
    }

    // Calculate total number of points and spacing
    size_t total_points = 1;
    Vector<N> spacing;
    for (size_t i = 0; i < N; ++i) {
        total_points *= npts[i];
        spacing[i] = dimensions[i] / (npts[i] > 1 ? npts[i] - 1 : 1);
    }

    // Initialize result array
    std::vector<Vector<N>> points(total_points);

    // Calculate min corner from center and dimensions
    Vector<N> min_corner;
    for (size_t i = 0; i < N; ++i) {
        min_corner[i] = center[i] - dimensions[i] / 2.0;
    }

    // Generate points
    for (size_t idx = 0; idx < total_points; ++idx) {
        Vector<N> point;
        size_t remaining = idx;

        // Calculate position in each dimension
        for (size_t dim = 0; dim < N; ++dim) {
            size_t stride = 1;
            for (size_t j = 0; j < dim; ++j) {
                stride *= npts[j];
            }
            size_t pos = (remaining / stride) % npts[dim];
            point[dim] = min_corner[dim] + pos * spacing[dim];
        }

        points[idx] = point;
    }

    return Serie<Vector<N>>(points);
}

} // namespace cartesian
} // namespace grid
} // namespace df
