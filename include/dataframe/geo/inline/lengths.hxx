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

// Compute length of a segment in 2D or 3D
template <typename T, size_t N>
T segment_length(const std::array<T, N> &v1, const std::array<T, N> &v2) {
    T sum = T{0};
    for (size_t i = 0; i < N; ++i) {
        T diff = v2[i] - v1[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

} // namespace detail

/**
 * Compute lengths of a series of line segments (2D or 3D)
 * @param vertices Serie of vertices
 * @param segments Serie of index pairs defining segments
 * @return Serie of segment lengths
 */
template <size_t N>
Serie<double> length(const Serie<Vector<N>> &vertices,
                     const Serie<iVector2> &segments) {
    if (vertices.empty() || segments.empty()) {
        return Serie<double>();
    }

    return segments.map([&vertices](const auto &segment, size_t) {
        const auto &v1 = vertices[segment[0]];
        const auto &v2 = vertices[segment[1]];
        return detail::segment_length<double, N>(v1, v2);
    });
}

// Binding functions for pipeline operations
template <size_t N>
auto bind_length(const Serie<iVector2> &segments) {
    return [&segments](const Serie<Vector<N>> &vertices) {
        return length<N>(vertices, segments);
    };
}

} // namespace df