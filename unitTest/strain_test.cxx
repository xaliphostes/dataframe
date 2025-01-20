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
#include <dataframe/functional/geo/strain.h>

TEST(strain, uniform_extension) {
    // Create a displacement field with uniform extension in x
    // 2x2x1 grid
    std::vector<double> displacements = {
        0.0, 0.0, 0.0,    // Point (0,0): no displacement
        1.0, 0.0, 0.0,    // Point (1,0): unit x displacement
        0.0, 0.0, 0.0,    // Point (0,1): no displacement
        1.0, 0.0, 0.0     // Point (1,1): unit x displacement
    };
    
    df::GenSerie<double> u(3, displacements);
    auto result = df::geo::strain(u, {2,2,1}, df::Serie::Array{1.0,1.0,1.0});
    
    EXPECT_EQ(result.count(), 4);
    EXPECT_EQ(result.itemSize(), 6);
    
    // Check strain components at point (0,0)
    auto strain00 = result.array(0);
    EXPECT_NEAR(strain00[0], 1.0, 1e-10);   // εxx = 1
    EXPECT_NEAR(strain00[1], 0.0, 1e-10);   // εyy = 0
    EXPECT_NEAR(strain00[2], 0.0, 1e-10);   // εzz = 0
    EXPECT_NEAR(strain00[3], 0.0, 1e-10);   // εxy = 0
    EXPECT_NEAR(strain00[4], 0.0, 1e-10);   // εyz = 0
    EXPECT_NEAR(strain00[5], 0.0, 1e-10);   // εxz = 0
}

TEST(strain, shear) {
    // Create a displacement field with simple shear
    // 2x2x1 grid
    std::vector<double> displacements = {
        0.0, 0.0, 0.0,    // Point (0,0): no displacement
        0.0, 1.0, 0.0,    // Point (1,0): unit y displacement
        0.0, 0.0, 0.0,    // Point (0,1): no displacement
        0.0, 1.0, 0.0     // Point (1,1): unit y displacement
    };
    
    df::GenSerie<double> u(3, displacements);
    auto result = df::geo::strain(u, {2,2,1}, df::Serie::Array{1.0,1.0,1.0});
    
    // Check strain components at point (0,0)
    auto strain00 = result.array(0);
    EXPECT_NEAR(strain00[0], 0.0, 1e-10);    // εxx = 0
    EXPECT_NEAR(strain00[1], 0.0, 1e-10);    // εyy = 0
    EXPECT_NEAR(strain00[2], 0.0, 1e-10);    // εzz = 0
    EXPECT_NEAR(strain00[3], 0.5, 1e-10);    // εxy = 1/2
    EXPECT_NEAR(strain00[4], 0.0, 1e-10);    // εyz = 0
    EXPECT_NEAR(strain00[5], 0.0, 1e-10);    // εxz = 0
}

TEST(strain, pipe_operator) {
    // Simple 2D test case with pipe operator
    std::vector<double> displacements = {
        0.0, 0.0, 0.0,    // Point (0,0)
        1.0, 0.0, 0.0     // Point (1,0)
    };
    
    df::GenSerie<double> u(3, displacements);
    auto result = u | df::geo::make_strain({2,1,1}, df::Serie::Array{1.0,1.0,1.0});
    
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(result.itemSize(), 6);
}

TEST(strain, errors) {
    // Test wrong displacement field dimension
    df::GenSerie<double> u_wrong(2, {0.0, 0.0, 0.0, 0.0});
    EXPECT_THROW(
        df::geo::strain(u_wrong, {2,1,1}, df::Serie::Array{1.0,1.0,1.0}),
        std::invalid_argument
    );
    
    // Test mismatched grid size
    df::GenSerie<double> u(3, {0.0,0.0,0.0, 1.0,0.0,0.0});
    EXPECT_THROW(
        df::geo::strain(u, {3,1,1}, df::Serie::Array{1.0,1.0,1.0}),
        std::invalid_argument
    );
}

RUN_TESTS();