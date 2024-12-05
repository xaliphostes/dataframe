/*
 * Copyright (c) 2023 fmaerten@gmail.com
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

#include <dataframe/functional/algebra/norm.h>
#include <dataframe/functional/geo/areas.h>
#include <dataframe/functional/geo/normals.h>
#include <dataframe/functional/math/div.h>

namespace df {
namespace geo {

Array create(const Array &v1, const Array &v2);

Array cross(const Array &v, const Array &w);

Serie normals(const Serie &positions, const Serie &indices) {
    if (!positions.isValid() || !indices.isValid()) {
        return Serie();
    }

    //   3D points                     triangles
    if (positions.itemSize() != 3 || indices.itemSize() != 3) {
        return Serie();
    }

    Array data = createArray(indices.count() * 3, 0);

    uint32_t i = 0;
    indices.forEach([&](const Array &t, uint32_t index) {
        auto v1 = positions.value(t[0]);
        auto v2 = positions.value(t[1]);
        auto v3 = positions.value(t[2]);
        auto n = cross(create(v1, v2), create(v1, v3));
        data[i++] = n[0];
        data[i++] = n[1];
        data[i++] = n[2];
    });

    return Serie(3, data);
}

Array create(const Array &v1, const Array &v2) {
    return Array{v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
}

Array cross(const Array &v, const Array &w) {
    double x = v[1] * w[2] - v[2] * w[1];
    double y = v[2] * w[0] - v[0] * w[2];
    double z = v[0] * w[1] - v[1] * w[0];
    return Array{x, y, z};
}

} // namespace geo
} // namespace df
