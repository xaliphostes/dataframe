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
#include <dataframe/functional/geo/areas.h>

TEST(areas, test1) {
    // Create positions for a simple geometry
    // Let's make 2 triangles sharing an edge
    auto positions = df::geo::Positions(3, {
                                               0.0, 0.0, 0.0, // vertex 0
                                               1.0, 0.0, 0.0, // vertex 1
                                               0.5, 1.0, 0.0, // vertex 2
                                               1.5, 1.0, 0.0  // vertex 3
                                           });

    // Define triangles using indices
    // Triangle 1: vertices 0,1,2
    // Triangle 2: vertices 1,3,2
    auto indices = df::geo::Indices(3, {
                                           0, 1, 2, // first triangle
                                           1, 3, 2  // second triangle
                                       });

    // Calculate areas
    auto areas = df::geo::areas(positions, indices);

    // Both triangles should have area = 0.5
    // (base=1.0, height=1.0, area=base*height/2)
    EXPECT_NEAR(areas.value(0), 0.5, 1e-10);
    EXPECT_NEAR(areas.value(1), 0.5, 1e-10);
}

TEST(areas, test2) {
    // Create a more complex geometry
    // Let's test with triangles of different sizes
    auto positions = df::geo::Positions(3, {
                                               0.0, 0.0, 0.0, // vertex 0
                                               2.0, 0.0, 0.0, // vertex 1
                                               1.0, 2.0, 0.0, // vertex 2
                                               4.0, 0.0, 0.0, // vertex 3
                                               3.0, 3.0, 0.0  // vertex 4
                                           });

    auto indices = df::geo::Indices(3, {
                                           0, 1, 2, // triangle 1
                                           1, 3, 4  // triangle 2
                                       });

    auto areas = df::geo::areas(positions, indices);

    // First triangle: base=2.0, height=2.0, area=2.0
    EXPECT_NEAR(areas.value(0), 2.0, 1e-10);

    // Second triangle: area should be larger
    // base=2.0, height=3.0, area=3.0
    EXPECT_NEAR(areas.value(1), 3.0, 1e-10);
}

TEST(areas, errors) {
    // Test empty positions
    df::geo::Positions empty_pos;
    auto indices = df::geo::Indices(3, {0, 1, 2});
    EXPECT_THROW(df::geo::areas(empty_pos, indices), std::invalid_argument);

    // Test empty indices
    // auto positions = df::geo::Positions(3, {0, 0, 0, 1, 0, 0, 0, 1, 0});
    // df::geo::Indices empty_indices;
    // EXPECT_THROW(df::geo::areas(positions, empty_indices),
    //              std::invalid_argument);

    // // Test invalid indices (out of bounds)
    // auto invalid_indices = df::geo::Indices(3, {0, 1, 5}); // 5 is out of bounds
    // EXPECT_THROW(df::geo::areas(positions, invalid_indices),
    //              std::invalid_argument);

    // // Test non-multiple-of-3 indices (triangles require 3 vertices each)
    // auto bad_count = df::geo::Indices(3, {0, 1, 2, 3}); // Not a multiple of 3
    // EXPECT_THROW(df::geo::areas(positions, bad_count), std::invalid_argument);
}

RUN_TESTS()