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

#include "TEST.h"
#include <dataframe/functional/geo/marching_triangle.h>

TEST(marching_triangle, example_1) {
    // Create topology
    df::GenSerie<uint32_t> topology(3, {0, 1, 2, 1, 3, 2, 2, 3, 4});

    // Create field values
    df::GenSerie<double> field(1, {0.0, 1.0, 0.5, 0.8, 0.2});

    // Create marching triangles instance
    df::geo::MarchingTriangles<double> algo;
    algo.setup(topology);

    // Generate isolines
    auto result = algo.isolines(field, 0.5);

    // Access results
    for (uint32_t i = 0; i < result.edges.count(); ++i) {
        auto edge = result.edges.array(i);
        double t = result.values.value(i);
        std::cout << "Edge: " << edge[0] << " - " << edge[1] << ", t = " << t
                  << "\n";
    }
}

TEST(marching_triangle, example_2) {
    // Create topology
    df::GenSerie<uint32_t> topology(3, {
        0, 1, 2,
        1, 3, 2,
        2, 3, 4
    });

    // Create vertex positions (3D points)
    df::GenSerie<double> vertices(3, {
        0.0, 0.0, 0.0,  // vertex 0
        1.0, 0.0, 0.0,  // vertex 1
        0.5, 1.0, 0.0,  // vertex 2
        1.5, 1.0, 0.0,  // vertex 3
        0.75, 2.0, 0.0  // vertex 4
    });

    // Create field values
    df::GenSerie<double> field(1, {
        0.0, 1.0, 0.5, 0.8, 0.2  // One value per vertex
    });

    // Create marching triangles instance
    df::geo::MarchingTriangles<double> marcher;
    marcher.setup(topology);

    // Compute isocontours directly
    auto contours = marcher.isocontours(field, vertices, 0.5);

    // Access results
    for (uint32_t i = 0; i < contours.points.count(); ++i) {
        auto point = contours.points.array(i);
        std::cout << "Point " << i << ": ("
                 << point[0] << ", " 
                 << point[1] << ", " 
                 << point[2] << ")\n";
    }

    for (uint32_t i = 0; i < contours.segments.count(); ++i) {
        auto segment = contours.segments.array(i);
        std::cout << "Segment " << i << ": "
                 << segment[0] << " - " 
                 << segment[1] << "\n";
    }
}

RUN_TESTS();