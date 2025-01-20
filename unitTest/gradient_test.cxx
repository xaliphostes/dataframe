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
#include <dataframe/functional/geo/gradient.h>

TEST(gradient, one_dimensional) {
    // 1D case: variation along x
    df::GenSerie<double> field1d(1, {0.0, 1.0, 2.0});
    auto grad1d = df::geo::gradient(field1d, {3, 1, 1}, {1.0, 1.0, 1.0});

    EXPECT_EQ(grad1d.count(), 3);
    EXPECT_EQ(grad1d.itemSize(), 3);

    // Check interior point
    auto grad_mid = grad1d.array(1);
    EXPECT_NEAR(grad_mid[0], 1.0, 1e-10); // df/dx = 1
    EXPECT_NEAR(grad_mid[1], 0.0, 1e-10); // df/dy = 0
    EXPECT_NEAR(grad_mid[2], 0.0, 1e-10); // df/dz = 0
}

TEST(gradient, two_dimensional) {
    // 2D case: 2x2 grid
    std::vector<double> values = {
        0.0, 1.0, // row 0
        2.0, 3.0  // row 1
    };
    df::GenSerie<double> field2d(1, values);
    auto grad2d = df::geo::gradient(field2d, {2, 2, 1}, {1.0, 1.0, 1.0});

    EXPECT_EQ(grad2d.count(), 4);
    EXPECT_EQ(grad2d.itemSize(), 3);

    // Check gradients at point (0,0)
    auto grad00 = grad2d.array(0);
    EXPECT_NEAR(grad00[0], 1.0, 1e-10); // df/dx = 1
    EXPECT_NEAR(grad00[1], 2.0, 1e-10); // df/dy = 2
    EXPECT_NEAR(grad00[2], 0.0, 1e-10); // df/dz = 0
}

TEST(gradient, three_dimensional) {
    // 3D case: 2x2x2 grid
    std::vector<double> values = {
        0.0, 1.0, // z=0, y=0
        2.0, 3.0, // z=0, y=1
        4.0, 5.0, // z=1, y=0
        6.0, 7.0  // z=1, y=1
    };
    df::GenSerie<double> field3d(1, values);
    auto grad3d = df::geo::gradient(field3d, {2, 2, 2}, {1.0, 1.0, 1.0});

    EXPECT_EQ(grad3d.count(), 8);
    EXPECT_EQ(grad3d.itemSize(), 3);

    // Check gradients at point (0,0,0)
    auto grad000 = grad3d.array(0);
    EXPECT_NEAR(grad000[0], 1.0, 1e-10); // df/dx = 1
    EXPECT_NEAR(grad000[1], 2.0, 1e-10); // df/dy = 2
    EXPECT_NEAR(grad000[2], 4.0, 1e-10); // df/dz = 4
}

TEST(gradient, non_uniform_spacing) {
    // Test with different spacing in each direction
    df::GenSerie<double> field(1, {0.0, 2.0});
    auto grad =
        df::geo::gradient(field, {2, 1, 1}, {2.0, 1.0, 1.0}); // dx = 2.0

    auto grad0 = grad.array(0);
    EXPECT_NEAR(grad0[0], 1.0, 1e-10); // (2.0 - 0.0)/2.0 = 1.0
}

TEST(gradient, pipe_operator_1D) {
    // Create a simple 1D field with 3 points
    df::GenSerie<double> field(1, {0.0, 1.0, 2.0});

    auto result = field | df::geo::make_gradient(
                              {3, 1, 1}, df::Serie::Array{1.0, 1.0, 1.0});
    EXPECT_EQ(result.count(), 3);
    EXPECT_EQ(result.itemSize(), 3);

    // Check first point gradient
    auto grad0 = result.array(0);
    EXPECT_NEAR(grad0[0], 1.0, 1e-10); // Forward difference
    EXPECT_NEAR(grad0[1], 0.0, 1e-10);
    EXPECT_NEAR(grad0[2], 0.0, 1e-10);
}

TEST(gradient, pipe_operator_2D) {
    // 2D case: 2x2 grid with linear variation
    std::vector<double> values = {
        0.0, 1.0, // row 0: x varies from 0 to 1
        2.0, 3.0  // row 1: x varies from 2 to 3
    };
    df::GenSerie<double> field2d(1, values);

    // Using pipe operator with 2D grid
    auto result2d =
        field2d | df::geo::make_gradient({2, 2, 1}, df::Serie::Array{1.0, 1.0, 1.0});

    EXPECT_EQ(result2d.count(), 4);
    EXPECT_EQ(result2d.itemSize(), 3);

    // Check gradients at bottom-left point (0,0)
    auto grad00 = result2d.array(0);
    EXPECT_NEAR(grad00[0], 1.0, 1e-10); // df/dx = 1 (x difference)
    EXPECT_NEAR(grad00[1], 2.0, 1e-10); // df/dy = 2 (y difference)
    EXPECT_NEAR(grad00[2], 0.0, 1e-10); // df/dz = 0 (2D case)
}

TEST(gradient, pipe_operator_3D) {
    // 3D case: 2x2x2 grid with linear variation
    std::vector<double> values = {
        0.0, 1.0, // z=0, y=0, x varies
        2.0, 3.0, // z=0, y=1, x varies
        4.0, 5.0, // z=1, y=0, x varies
        6.0, 7.0  // z=1, y=1, x varies
    };
    df::GenSerie<double> field3d(1, values);

    // Using pipe operator with 3D grid
    auto result3d =
        field3d | df::geo::make_gradient({2, 2, 2}, df::Serie::Array{1.0, 1.0, 1.0});

    EXPECT_EQ(result3d.count(), 8);
    EXPECT_EQ(result3d.itemSize(), 3);

    // Check gradients at origin point (0,0,0)
    auto grad000 = result3d.array(0);
    EXPECT_NEAR(grad000[0], 1.0, 1e-10); // df/dx = 1 (x varies by 1)
    EXPECT_NEAR(grad000[1], 2.0, 1e-10); // df/dy = 2 (y varies by 2)
    EXPECT_NEAR(grad000[2], 4.0, 1e-10); // df/dz = 4 (z varies by 4)

    // Check gradients at point (1,1,1) - last point
    auto grad111 = result3d.array(7);
    EXPECT_NEAR(grad111[0], 1.0, 1e-10); // df/dx still = 1
    EXPECT_NEAR(grad111[1], 2.0, 1e-10); // df/dy still = 2
    EXPECT_NEAR(grad111[2], 4.0, 1e-10); // df/dz still = 4
}

RUN_TESTS();