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
#include <dataframe/functional/geo/curve_curvature.h>
#include <cmath>

// Test circular arc (constant curvature)
TEST(Curvature, CircularArc) {
    std::vector<double> points;
    const int num_points = 20;
    const double radius = 2.0;
    const double expected_curvature = 1.0 / radius;
    
    // Create points on a circular arc in the xy plane
    for (int i = 0; i < num_points; ++i) {
        double theta = M_PI * i / (num_points - 1);  // 0 to π
        double x = radius * std::cos(theta);
        double y = radius * std::sin(theta);
        double z = 0.0;
        
        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
    }
    
    MSG("Testing curvature of circular arc");
    df::GenSerie<double> curve(3, points);
    auto result = df::geo::curve_curvature(curve);
    
    // Check that curvature is approximately constant
    for (uint32_t i = 1; i < result.count() - 1; ++i) {  // Skip endpoints
        EXPECT_NEAR(result.value(i), expected_curvature, 1e-2);
    }
}

// Test straight line (zero curvature)
TEST(Curvature, StraightLine) {
    std::vector<double> points;
    const int num_points = 10;
    
    // Create points on a straight line
    for (int i = 0; i < num_points; ++i) {
        double t = static_cast<double>(i) / (num_points - 1);
        points.push_back(t);        // x = t
        points.push_back(2.0 * t);  // y = 2t
        points.push_back(3.0 * t);  // z = 3t
    }
    
    MSG("Testing curvature of straight line");
    df::GenSerie<double> curve(3, points);
    auto result = df::geo::curve_curvature(curve);
    
    // Check that curvature is approximately zero
    for (uint32_t i = 0; i < result.count(); ++i) {
        EXPECT_NEAR(result.value(i), 0.0, 1e-5);
    }
}

// Test helix (constant curvature)
TEST(Curvature, Helix) {
    std::vector<double> points;
    const int num_points = 50;
    const double radius = 1.0;
    const double pitch = 0.5;
    
    // Create points on a helix
    for (int i = 0; i < num_points; ++i) {
        double t = 4.0 * M_PI * i / (num_points - 1);  // 0 to 4π
        double x = radius * std::cos(t);
        double y = radius * std::sin(t);
        double z = pitch * t;
        
        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
    }
    
    MSG("Testing curvature of helix");
    df::GenSerie<double> curve(3, points);
    auto result = df::geo::curve_curvature(curve);
    
    // Expected curvature for a helix
    const double expected_curvature = radius / (radius * radius + pitch * pitch);
    
    // Check that curvature is approximately constant
    for (uint32_t i = 1; i < result.count() - 1; ++i) {  // Skip endpoints
        EXPECT_NEAR(result.value(i), expected_curvature, 1e-2);
    }
}

// Test error conditions
TEST(Curvature, ErrorHandling) {
    MSG("Testing error handling");
    
    // Test wrong dimension
    std::vector<double> points_2d = {0.0, 0.0, 1.0, 1.0};
    df::GenSerie<double> curve_2d(2, points_2d);
    EXPECT_THROW(
        df::geo::curve_curvature(curve_2d),
        std::invalid_argument
    );
    
    // Test too few points
    std::vector<double> points_few = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
    df::GenSerie<double> curve_few(3, points_few);
    EXPECT_THROW(
        df::geo::curve_curvature(curve_few),
        std::invalid_argument
    );
    
    // Test mismatched parameterization
    std::vector<double> points_valid = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0};
    df::GenSerie<double> curve_valid(3, points_valid);
    df::GenSerie<double> wrong_param(1, {0.0, 1.0});  // Wrong count
    EXPECT_THROW(
        df::geo::curve_curvature(curve_valid, wrong_param),
        std::invalid_argument
    );
}

// Test with custom parameterization
// TEST(Curvature, CustomParameterization) {
//     std::vector<double> points;
//     std::vector<double> params;
//     const int num_points = 10;
    
//     // Create unevenly spaced points on a circle
//     for (int i = 0; i < num_points; ++i) {
//         // Use squared parameter to create uneven spacing
//         double t = static_cast<double>(i * i) / ((num_points - 1) * (num_points - 1));
//         double theta = 2.0 * M_PI * t;
        
//         points.push_back(std::cos(theta));
//         points.push_back(std::sin(theta));
//         points.push_back(0.0);
        
//         params.push_back(t);
//     }
    
//     MSG("Testing curvature with custom parameterization");
//     df::GenSerie<double> curve(3, points);
//     df::GenSerie<double> parameterization(1, params);
    
//     auto result = df::geo::curve_curvature(curve, parameterization);
    
//     // Check that curvature is approximately 1 (unit circle)
//     for (uint32_t i = 1; i < result.count() - 1; ++i) {  // Skip endpoints
//         EXPECT_NEAR(result.value(i), 1.0, 1e-1);
//     }
// }

// Add main function to run all tests
RUN_TESTS()
