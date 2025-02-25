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

#include "../../TEST.h"
#include <dataframe/geo/grid/from_points.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(grid, from_dims_2d) {
    MSG("Testing 2D grid generation with dimensions");

    // Test basic 2x2 grid centered at origin
    iVector2 npts{2, 2};
    Vector2 center{0.0, 0.0};
    Vector2 dims{2.0, 2.0};
    auto grid = grid::cartesian::from_dims(npts, center, dims);

    std::vector<Vector2> expected{
        {-1.0, -1.0}, {1.0, -1.0}, {-1.0, 1.0}, {1.0, 1.0}};
    EXPECT_EQ(grid.size(), 4);
    grid.forEach([expected](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, expected[index], 1e-10);
    });

    // Test non-uniform grid
    iVector2 npts2{2, 3};
    Vector2 center2{1.0, 1.0};
    Vector2 dims2{2.0, 4.0};
    auto grid2 = grid::cartesian::from_dims(npts2, center2, dims2);

    std::vector<Vector2> expected2{{0.0, -1.0}, {2.0, -1.0}, {0.0, 1.0},
                                   {2.0, 1.0},  {0.0, 3.0},  {2.0, 3.0}};
    EXPECT_EQ(grid2.size(), 6);
    grid2.forEach([expected2](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, expected2[index], 1e-10);
    });
}

TEST(grid, from_dims_3d) {
    MSG("Testing 3D grid generation with dimensions");

    // Test 2x2x2 grid centered at origin
    iVector3 npts{2, 2, 2};
    Vector3 center{0.0, 0.0, 0.0};
    Vector3 dims{2.0, 2.0, 2.0};
    auto grid = grid::cartesian::from_dims(npts, center, dims);

    std::vector<Vector3> expected{{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0},
                                  {-1.0, 1.0, -1.0},  {1.0, 1.0, -1.0},
                                  {-1.0, -1.0, 1.0},  {1.0, -1.0, 1.0},
                                  {-1.0, 1.0, 1.0},   {1.0, 1.0, 1.0}};
    EXPECT_EQ(grid.size(), 8);
    grid.forEach([expected](const Vector3 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, expected[index], 1e-10);
    });
}

TEST(grid, from_points_2d) {
    MSG("Testing 2D grid generation from points");

    // Test basic 2x2 grid
    iVector2 npts{2, 2};
    std::vector<double> p1{0.0, 0.0};
    std::vector<double> p2{1.0, 1.0};
    auto grid = grid::cartesian::from_points(npts, p1, p2);

    std::vector<Vector2> expected{
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};
    EXPECT_EQ(grid.size(), 4);
    grid.forEach([expected](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, expected[index], 1e-10);
    });

    // Test reversed points (should give same grid)
    auto grid2 = grid::cartesian::from_points(npts, p2, p1);
    grid2.forEach([expected](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, expected[index], 1e-10);
    });
}

TEST(grid, edge_cases) {
    MSG("Testing grid generation edge cases");

    // Test single point grid
    iVector2 single_npts{1, 1};
    Vector2 center{0.0, 0.0};
    Vector2 dims{1.0, 1.0};
    auto single_grid = grid::cartesian::from_dims(single_npts, center, dims);
    df::print(single_grid);
    EXPECT_EQ(single_grid.size(), 1);
    // EXPECT_ARRAY_NEAR(single_grid[0], Vector2({0.0, 0.0}), 1e-10);

    // Test line grid (1xN)
    iVector2 line_npts{1, 3};
    auto line_grid = grid::cartesian::from_dims(line_npts, center, dims);
    df::print(line_grid);
    EXPECT_EQ(line_grid.size(), 3);
}

TEST(grid, error_cases) {
    MSG("Testing grid generation error cases");

    // Test invalid number of points
    iVector2 invalid_npts{0, 2};
    Vector2 center{0.0, 0.0};
    Vector2 dims{1.0, 1.0};
    EXPECT_THROW(grid::cartesian::from_dims(invalid_npts, center, dims),
                 std::invalid_argument);

    // Test invalid dimensions
    iVector2 npts{2, 2};
    Vector2 invalid_dims{-1.0, 1.0};
    EXPECT_THROW(grid::cartesian::from_dims(npts, center, invalid_dims),
                 std::invalid_argument);

    // Test mismatched dimensions in from_points
    iVector2 npts2{2, 2};
    std::vector<double> p1{0.0}; // Wrong size
    std::vector<double> p2{1.0, 1.0};
    EXPECT_THROW(grid::cartesian::from_points(npts2, p1, p2),
                 std::invalid_argument);
}

RUN_TESTS()