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
namespace geo {

inline Array<double> create(const Array<double> &v1, const Array<double> &v2) {
    return Array<double>{v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
}

inline Array<double> cross(const Array<double> &v, const Array<double> &w) {
    double x = v[1] * w[2] - v[2] * w[1];
    double y = v[2] * w[0] - v[0] * w[2];
    double z = v[0] * w[1] - v[1] * w[0];
    return Array<double>{x, y, z};
}

inline Attribute normals(const Positions &positions, const Indices &indices) {
    if (!positions.isValid() || !indices.isValid()) {
        throw std::runtime_error("Invalid positions or indices");
    }

    //   3D points                     triangles
    if (positions.itemSize() != 3 || indices.itemSize() != 3) {
        throw std::runtime_error("itemSize must be 3 for both positions and indices");
    }

    Array<double> data = createArray<double>(indices.count() * 3, 0);

    uint32_t i = 0;
    df::forEach([&](const Array<Indices::value_type> &t, uint32_t index) {
        if (t[0] >= positions.count() || t[1] >= positions.count() || t[2] >= positions.count()) {
            throw std::runtime_error("Invalid index");
        }
        auto v1 = positions.array(t[0]);
        auto v2 = positions.array(t[1]);
        auto v3 = positions.array(t[2]);
        auto n = cross(create(v1, v2), create(v1, v3));
        data[i++] = n[0];
        data[i++] = n[1];
        data[i++] = n[2];
    }, indices);

    return Attribute(3, data);
}

} // namespace geo
} // namespace df
