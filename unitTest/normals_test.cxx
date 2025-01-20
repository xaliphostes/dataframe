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
#include <dataframe/functional/geo/normals.h>

TEST(normals, test1) {
    // Create positions for a simple geometry
    // Let's make 2 triangles sharing an edge
    df::GenSerie<double> positions(3, {
        0.0, 0.0, 0.0,  // vertex 0
        1.0, 0.0, 0.0,  // vertex 1
        0.0, 1.0, 0.0,  // vertex 2
        1.0, 1.0, 0.0   // vertex 3
    });

    // Define triangles using indices
    df::GenSerie<uint32_t> indices(3, {
        0, 1, 2,  // first triangle
        1, 3, 2   // second triangle
    });

    // Using pipe operator
    auto normals = df::pipe(positions, df::geo::make_normals(indices));
    
    // Both triangles should have normal = (0,0,1)
    EXPECT_NEAR(normals.array(0)[0], 0.0, 1e-10); // x
    EXPECT_NEAR(normals.array(0)[1], 0.0, 1e-10); // y
    EXPECT_NEAR(normals.array(0)[2], 1.0, 1e-10); // z

    EXPECT_NEAR(normals.array(1)[0], 0.0, 1e-10); // x
    EXPECT_NEAR(normals.array(1)[1], 0.0, 1e-10); // y
    EXPECT_NEAR(normals.array(1)[2], 1.0, 1e-10); // z
}

TEST(normals, errors) {
    // Test with wrong itemSize for positions
    df::GenSerie<double> bad_pos(2, {0,0, 1,0, 0,1}); // itemSize=2 instead of 3
    df::GenSerie<uint32_t> indices(3, {0,1,2});
    EXPECT_THROW(df::pipe(bad_pos, df::geo::make_normals(indices)), std::runtime_error);

    // Test with wrong itemSize for indices
    df::GenSerie<double> positions(3, {0,0,0, 1,0,0, 0,1,0});
    df::GenSerie<uint32_t> bad_idx(2, {0,1, 1,2}); // itemSize=2 instead of 3
    EXPECT_THROW(df::pipe(positions, df::geo::make_normals(bad_idx)), std::runtime_error);

    // Test with invalid index
    df::GenSerie<uint32_t> invalid_idx(3, {0,1,10}); // 10 is out of bounds
    EXPECT_THROW(df::pipe(positions, df::geo::make_normals(invalid_idx)), std::runtime_error);
}

RUN_TESTS()