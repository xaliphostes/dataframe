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

inline std::tuple<Positions3, Triangles> generateSphere(double radius, size_t nLon, size_t nLat) {
    Positions3 vertices;
    Triangles triangles;

    // Generate vertices
    for (size_t j = 0; j <= nLat; ++j) {
        double lat = M_PI * (-0.5 + static_cast<double>(j) / nLat);
        double cosLat = std::cos(lat);
        double sinLat = std::sin(lat);

        for (size_t i = 0; i <= nLon; ++i) {
            double lon = 2 * M_PI * static_cast<double>(i) / nLon;
            double cosLon = std::cos(lon);
            double sinLon = std::sin(lon);

            double x = radius * cosLat * cosLon;
            double y = radius * cosLat * sinLon;
            double z = radius * sinLat;

            vertices.add({x, y, z});
        }
    }

    // Generate triangles
    for (size_t j = 0; j < nLat; ++j) {
        size_t startRow = j * (nLon + 1);
        size_t nextRow = (j + 1) * (nLon + 1);

        for (size_t i = 0; i < nLon; ++i) {
            triangles.add({static_cast<uint>(startRow + i),
                           static_cast<uint>(startRow + i + 1),
                           static_cast<uint>(nextRow + i)});

            triangles.add({static_cast<uint>(nextRow + i),
                           static_cast<uint>(startRow + i + 1),
                           static_cast<uint>(nextRow + i + 1)});
        }
    }

    return {vertices, triangles};
}

} // namespace df