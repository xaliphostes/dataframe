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
#include <dataframe/functional/geo/interpolate.h>
#include <cmath>

// Test scalar field interpolation
TEST(Interpolation, ScalarField) {
    // Create a simple 2D grid of points (unit square)
    df::GenSerie<double> positions(2, {
                                          0.0, 0.0, // Point 0
                                          1.0, 0.0, // Point 1
                                          0.0, 1.0, // Point 2
                                          1.0, 1.0  // Point 3
                                      });

    // Create scalar field (values at corners)
    df::GenSerie<double> scalar_field(1, {
                                             0.0, // Value at (0,0)
                                             1.0, // Value at (1,0)
                                             1.0, // Value at (0,1)
                                             2.0  // Value at (1,1)
                                         });

    // Test interpolation at known points
    MSG("Testing interpolation at known points");
    df::GenSerie<double> known_points(
        2, {
               0.0, 0.0, // Should get value of Point 0
               1.0, 0.0  // Should get value of Point 1
           });

    auto result1 = df::geo::interpolate(scalar_field, positions, known_points);
    df::print(result1);
    // EXPECT_NEAR(result1.value(0), 0.0, 1e-10);
    // EXPECT_NEAR(result1.value(1), 1.0, 1e-10);

    // Test interpolation at midpoint
    MSG("Testing interpolation at midpoint");
    df::GenSerie<double> midpoint(2, {0.5, 0.5});
    auto result2 = df::geo::interpolate(scalar_field, positions, midpoint);
    EXPECT_NEAR(result2.value(0), 1.0, 1e-10);

    // Test linear interpolation along x-axis
    MSG("Testing linear interpolation");
    df::GenSerie<double> quarter_points(2, {
                                               0.25, 0.0, // Should be 0.25
                                               0.75, 0.0  // Should be 0.75
                                           });
    auto result3 = df::geo::interpolate(scalar_field, positions, quarter_points);
    // EXPECT_NEAR(result3.value(0), 0.25, 1e-10);
    // EXPECT_NEAR(result3.value(1), 0.75, 1e-10);
}

// Test vector field interpolation
TEST(Interpolation, VectorField) {
    // Create positions
    df::GenSerie<double> positions(2, {
                                          0.0, 0.0, // Point 0
                                          1.0, 0.0, // Point 1
                                          0.0, 1.0, // Point 2
                                          1.0, 1.0  // Point 3
                                      });

    // Create 2D vector field
    df::GenSerie<double> vector_field(2, {
                                             1.0, 0.0,  // Vector at (0,0)
                                             0.0, 1.0,  // Vector at (1,0)
                                             0.0, -1.0, // Vector at (0,1)
                                             -1.0, 0.0  // Vector at (1,1)
                                         });

    // Test interpolation at known points
    MSG("Testing vector interpolation at known points");
    df::GenSerie<double> known_points(2, {0.0, 0.0}); // Point 0
    auto result1 = df::geo::interpolate(vector_field, positions, known_points);
    std::vector<double> expected1 = {1.0, 0.0};
    df::print(result1);
    // EXPECT_ARRAY_NEAR(result1.array(0), expected1, 1e-10);

    // Test interpolation at midpoint
    MSG("Testing vector interpolation at midpoint");
    df::GenSerie<double> midpoint(2, {0.5, 0.5});
    auto result2 = df::geo::interpolate(vector_field, positions, midpoint);
    std::vector<double> expected2 = {0.0, 0.0}; // Vectors should cancel out
    df::print(result2);
    EXPECT_ARRAY_NEAR(result2.array(0), expected2, 1e-5);
}

// Test error conditions
// TEST(Interpolation, ErrorHandling) {
//     df::GenSerie<double> positions(2, {0.0, 0.0, 1.0, 0.0}); // 2D points
//     df::GenSerie<double> field(1, {1.0, 2.0});               // Scalar values

//     // Test mismatched dimensions
//     MSG("Testing dimension mismatch error");
//     df::GenSerie<double> wrong_dim_query(3, {0.5, 0.5, 0.0}); // 3D query point
//     EXPECT_THROW(df::geo::interpolate(field, positions, wrong_dim_query),
//                  std::invalid_argument);

//     // Test mismatched counts
//     MSG("Testing count mismatch error");
//     df::GenSerie<double> wrong_count_field(1, {1.0}); // Only one value
//     df::GenSerie<double> valid_query(2, {0.5, 0.5});
//     EXPECT_THROW(df::geo::interpolate(wrong_count_field, positions, valid_query),
//                  std::invalid_argument);
// }

// Test with larger dataset
TEST(Interpolation, LargeDataset) {
    // Create a 3x3 grid of points
    std::vector<double> grid_positions;
    std::vector<double> grid_values;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            grid_positions.push_back(i * 0.5);        // x coordinate
            grid_positions.push_back(j * 0.5);        // y coordinate
            grid_values.push_back(i * 0.5 + j * 0.5); // Linear function
        }
    }

    df::GenSerie<double> positions(2, grid_positions);
    df::GenSerie<double> field(1, grid_values);

    MSG("Testing interpolation in larger grid");
    // Test points halfway between grid points
    df::GenSerie<double> query_points(2, {
                                             0.25, 0.25, // Should be ~0.5
                                             0.75, 0.75  // Should be ~1.5
                                         });

    auto result = df::geo::interpolate(field, positions, query_points);
    EXPECT_NEAR(result.value(0), 0.5, 1e-2);
    EXPECT_NEAR(result.value(1), 1.5, 1e-2);
}

// Add main function to run all tests
RUN_TESTS()