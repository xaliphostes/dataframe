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
#include <dataframe/functional/geo/cartesian_grid.h>

TEST(cartesian_grid, from_dims) {
    // Test 1D grid (line)
    auto line =
        df::grid::cartesian::from_dims<double>({5}, {0, 0, 0}, {1, 0, 0});
    EXPECT_EQ(line.count(), 5);
    EXPECT_EQ(line.itemSize(), 3);
    EXPECT_ARRAY_EQ(line.asArray(),
                    df::Serie(2, {-0.5000, 0.0000, 0.0000, -0.2500, 0.0000,
                                  0.0000, 0.0000, 0.0000, 0.0000, 0.2500,
                                  0.0000, 0.0000, 0.5000, 0.0000, 0.0000})
                        .asArray());

    // Test 2D grid
    auto grid =
        df::grid::cartesian::from_dims<double>({3, 3}, {0, 0, 0}, {1, 1, 0});
    EXPECT_EQ(grid.count(), 9);
    EXPECT_EQ(grid.itemSize(), 3);
    EXPECT_ARRAY_EQ(
        grid.asArray(),
        df::Serie(3,
                  {-0.5000, -0.5000, 0.0000,  0.0000, -0.5000, 0.0000, 0.5000,
                   -0.5000, 0.0000,  -0.5000, 0.0000, 0.0000,  0.0000, 0.0000,
                   0.0000,  0.5000,  0.0000,  0.0000, -0.5000, 0.5000, 0.0000,
                   0.0000,  0.5000,  0.0000,  0.5000, 0.5000,  0.0000})
            .asArray());

    // Test 3D grid
    auto cube =
        df::grid::cartesian::from_dims<double>({2, 2, 2}, {0, 0, 0}, {1, 1, 1});
    EXPECT_EQ(cube.count(), 8);
    EXPECT_EQ(cube.itemSize(), 3);
    EXPECT_ARRAY_EQ(
        cube.asArray(),
        df::Serie(3, {-0.5000, -0.5000, -0.5000, 0.5000, -0.5000, -0.5000,
                      -0.5000, 0.5000,  -0.5000, 0.5000, 0.5000,  -0.5000,
                      -0.5000, -0.5000, 0.5000,  0.5000, -0.5000, 0.5000,
                      -0.5000, 0.5000,  0.5000,  0.5000, 0.5000,  0.5000})
            .asArray());
}

TEST(cartesian_grid, from_points) {
    // Test 1D grid (line)
    auto line =
        df::grid::cartesian::from_points<double>({5}, {0, 0, 0}, {1, 0, 0});
    EXPECT_EQ(line.count(), 5);
    EXPECT_EQ(line.itemSize(), 3);
    EXPECT_ARRAY_EQ(line.asArray(),
                    df::Serie(3, {0.0000, 0.0000, 0.0000, 0.2500, 0.0000,
                                  0.0000, 0.5000, 0.0000, 0.0000, 0.7500,
                                  0.0000, 0.0000, 1.0000, 0.0000, 0.0000})
                        .asArray());

    // Test error cases
    EXPECT_THROW(
        df::grid::cartesian::from_points<double>({}, {0, 0, 0}, {1, 1, 1}),
        std::invalid_argument);
    EXPECT_THROW(df::grid::cartesian::from_points<double>({2, 2, 2, 2},
                                                          {0, 0, 0}, {1, 1, 1}),
                 std::invalid_argument);
}

RUN_TESTS();