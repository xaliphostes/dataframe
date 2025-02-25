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

#include <dataframe/Serie.h>
#include <dataframe/geo/mesh/uv_mapping.h>
#include <dataframe/types.h>
#include <dataframe/utils/print.h>
#include <iostream>

int main() {
    std::vector<Vector3> vertices;
    std::vector<iVector3> triangles;

    // Parameters
    const int rings = 10;
    const int segments = 16;
    const double radius = 1.0;

    // Add top vertex
    vertices.push_back({0, 0, radius});

    // Generate rings of vertices
    for (int ring = 1; ring < rings; ring++) {
        double phi = M_PI * 0.5 * (double)ring / rings;
        for (int seg = 0; seg < segments; seg++) {
            double theta = 2.0 * M_PI * (double)seg / segments;
            double x = radius * sin(phi) * cos(theta);
            double y = radius * sin(phi) * sin(theta);
            double z = radius * cos(phi);
            vertices.push_back({x, y, z});
        }
    }

    // Generate triangles
    // Top cap
    for (uint seg = 0; seg < segments; seg++) {
        uint next = (seg + 1) % segments;
        triangles.push_back({0, seg + 1, next + 1});
    }

    // Ring triangles
    for (uint ring = 1; ring < rings - 1; ring++) {
        uint ringStart = 1 + (ring - 1) * segments;
        uint nextRingStart = ringStart + segments;
        for (uint seg = 0; seg < segments; seg++) {
            uint next = (seg + 1) % segments;
            triangles.push_back(
                {ringStart + seg, nextRingStart + seg, ringStart + next});
            triangles.push_back(
                {ringStart + next, nextRingStart + seg, nextRingStart + next});
        }
    }

    df::Mesh3D mesh(vertices, triangles);
    df::UVMapping uvMap = df::uvMapping(mesh);

    // Print results
    std::cout << "UV Coordinates:\n";
    df::print(uvMap.uvCoords);
    
    std::cout << "\nSeams:\n";
    df::print(uvMap.seams);

    return 0;
}
