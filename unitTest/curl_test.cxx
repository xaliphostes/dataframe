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
#include <cmath>
#include <dataframe/functional/geo/curl.h>

// Test 2D curl
TEST(Curl, VectorField2D) {
    // Create a simple rotational field in 2D
    // Field is (y, -x) which should have constant curl = -2
    const uint32_t grid_size = 5;
    std::vector<double> coords;
    std::vector<double> field_values;

    for (uint32_t i = 0; i < grid_size; ++i) {
        for (uint32_t j = 0; j < grid_size; ++j) {
            double x = -1.0 + 2.0 * i / (grid_size - 1);
            double y = -1.0 + 2.0 * j / (grid_size - 1);

            coords.push_back(x);
            coords.push_back(y);

            field_values.push_back(y);  // v_x = y
            field_values.push_back(-x); // v_y = -x
        }
    }

    MSG("Testing 2D rotational field");
    df::GenSerie<double> coordinates(2, coords);
    df::GenSerie<double> field(2, field_values);

    auto result = df::geo::curl(field, coordinates);

    // Check that curl is approximately -2 everywhere
    for (uint32_t i = 0; i < result.count(); ++i) {
        EXPECT_NEAR(result.value(i), -2.0, 1e-1);
    }
}

// Test 3D curl
TEST(Curl, VectorField3D) {
    // Create a regular 3D grid
    const uint32_t grid_size = 4;
    std::vector<double> coords;
    std::vector<double> field_values;

    // Create a 3D rotational field that rotates around z-axis
    // Field = (-y, x, 0), which should have curl = (0, 0, 2)
    for (uint32_t i = 0; i < grid_size; ++i) {
        for (uint32_t j = 0; j < grid_size; ++j) {
            for (uint32_t k = 0; k < grid_size; ++k) {
                double x = -1.0 + 2.0 * i / (grid_size - 1);
                double y = -1.0 + 2.0 * j / (grid_size - 1);
                double z = -1.0 + 2.0 * k / (grid_size - 1);

                // Store coordinates
                coords.push_back(x);
                coords.push_back(y);
                coords.push_back(z);

                // Field values: v = (-y, x, 0)
                field_values.push_back(-y);  // v_x = -y
                field_values.push_back(x);   // v_y = x
                field_values.push_back(0.0); // v_z = 0
            }
        }
    }

    MSG("Testing 3D rotational field");
    df::GenSerie<double> coordinates(3, coords);
    df::GenSerie<double> field(3, field_values);

    auto result = df::geo::curl(field, coordinates);
    // df::print(result);

    // Check curl components at each point
    // For v = (-y, x, 0), curl should be (0, 0, 2)
    for (uint32_t i = 0; i < result.count(); ++i) {
        auto curl_vec = result.array(i);
        auto pos = coordinates.array(i);

        // MSG("Testing curl at point (" + std::to_string(pos[0]) + ", " +
        //     std::to_string(pos[1]) + ", " + std::to_string(pos[2]) + ")");

        EXPECT_NEAR(curl_vec[0], 0.0, 1e-1); // x component
        EXPECT_NEAR(curl_vec[1], 0.0, 1e-1); // y component
        EXPECT_NEAR(curl_vec[2], 2.0, 1e-1); // z component should be 2
    }
}

// Test error conditions
TEST(Curl, ErrorHandling) {
    MSG("Testing dimension mismatch errors");

    // Create 2D data
    df::GenSerie<double> coords_2d(2, {0.0, 1.0, 1.0, 0.0});
    df::GenSerie<double> field_2d(2, {1.0, 0.0, 0.0, 1.0});

    // Create 3D data with wrong dimensions
    df::GenSerie<double> field_3d(3, {1.0, 0.0, 0.0});

    // Test mismatched field and coordinate dimensions
    EXPECT_THROW(df::geo::curl(field_3d, coords_2d), std::invalid_argument);

    // Test invalid coordinate dimensions
    df::GenSerie<double> invalid_coords(4, {0.0, 0.0, 0.0, 0.0});
    EXPECT_THROW(df::geo::curl(field_2d, invalid_coords), std::invalid_argument);

    // Test mismatched counts
    df::GenSerie<double> wrong_count_coords(2, {0.0, 0.0});
    EXPECT_THROW(df::geo::curl(field_2d, wrong_count_coords), std::invalid_argument);
}

// Add main function to run all tests
RUN_TESTS()