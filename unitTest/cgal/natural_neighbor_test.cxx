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

#include "../TEST.h"
#include <cmath>
#include <dataframe/geo/interpolation/natural_neighbor.h>

// Helper function to create a simple function for testing
double test_function(double x, double y) { return std::sin(x) * std::cos(y); }

TEST(NaturalNeighbor, BasicInterpolation) {
    // Create a simple square of points
    df::Serie<Vector2> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};

    df::Serie<double> values = {0.0, 1.0, 1.0, 2.0};

    // Test point in the middle
    df::Serie<Vector2> targets = {{0.5, 0.5}};

    auto result =
        df::natural_neighbor_2d(points, values, targets);

    // Middle point should be approximately the average of corners
    EXPECT_NEAR(result[0], 1.0, 1e-10);
}

TEST(NaturalNeighbor, ExactInterpolation) {
    // Test that interpolation exactly reproduces values at input points
    df::Serie<Vector2> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {0.5, 0.5}};

    df::Serie<double> values = {0.0, 1.0, 1.0, 2.0, 1.5};

    // Test interpolation at input points
    auto result =
        df::natural_neighbor_2d(points, values, points);

    for (size_t i = 0; i < values.size(); ++i) {
        EXPECT_NEAR(result[i], values[i], 1e-10);
    }
}

TEST(NaturalNeighbor, Continuity) {
    // Test continuity by checking nearby points
    df::Serie<Vector2> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};

    df::Serie<double> values = {0.0, 1.0, 1.0, 2.0};

    // Create two very close points
    double eps = 1e-6;
    df::Serie<Vector2> targets = {{0.5, 0.5}, {0.5 + eps, 0.5 + eps}};

    auto result =
        df::natural_neighbor_2d(points, values, targets);

    // Values should be very close for nearby points
    EXPECT_NEAR(result[0], result[1], eps);
}

TEST(NaturalNeighbor, LinearFunction) {
    // Test that interpolation exactly reproduces a linear function
    df::Serie<Vector2> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};

    // Linear function f(x,y) = 2x + 3y
    df::Serie<double> values = {
        0.0, // f(0,0) = 0
        2.0, // f(1,0) = 2
        3.0, // f(0,1) = 3
        5.0  // f(1,1) = 5
    };

    // Test points
    df::Serie<Vector2> targets = {
        {0.5, 0.5}, // Should be 2.5
        {0.3, 0.7}, // Should be 2.7
        {0.8, 0.2}  // Should be 2.2
    };

    auto result =
        df::natural_neighbor_2d(points, values, targets);

    // Check that interpolation reproduces linear function
    EXPECT_NEAR(result[0], 2.5, 1e-10);
    EXPECT_NEAR(result[1], 2.7, 1e-10);
    EXPECT_NEAR(result[2], 2.2, 1e-10);
}

TEST(NaturalNeighbor, SmoothFunction) {
    // Test interpolation of a smooth function
    const int grid_size = 5;
    df::Serie<Vector2> points;
    df::Serie<double> values;

    // Create grid of points
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            double x = i * (2.0 * M_PI / (grid_size - 1));
            double y = j * (2.0 * M_PI / (grid_size - 1));
            points.add({x, y});
            values.add(test_function(x, y));
        }
    }

    // Create test points
    df::Serie<Vector2> targets = {{M_PI / 4, M_PI / 4},
                                      {M_PI / 2, M_PI / 2},
                                      {3 * M_PI / 4, 3 * M_PI / 4}};

    auto result =
        df::natural_neighbor_2d(points, values, targets);

    // Check that interpolation is reasonably close to true function
    for (size_t i = 0; i < targets.size(); ++i) {
        double true_value = test_function(targets[i][0], targets[i][1]);
        EXPECT_NEAR(result[i], true_value,
                    0.1); // Allow some interpolation error
    }
}

TEST(NaturalNeighbor, EdgeCases) {
    df::Serie<Vector2> points = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};

    df::Serie<double> values = {0.0, 1.0, 1.0, 2.0};

    // Test points outside the convex hull
    df::Serie<Vector2> outside_points = {{-0.5, -0.5}, {1.5, 1.5}};

    // Should not throw but extrapolate based on nearest neighbors
    EXPECT_NO_THROW({
        auto result = df::natural_neighbor_2d(points, values,
                                                                outside_points);
    });
}

TEST(NaturalNeighbor, Collinear) {
    // Test with collinear points
    df::Serie<Vector2> points = {{0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}};

    df::Serie<double> values = {0.0, 0.5, 1.0};

    df::Serie<Vector2> targets = {{0.25, 0.0}, {0.75, 0.0}};

    auto result =
        df::natural_neighbor_2d(points, values, targets);

    // Should still interpolate linearly along the line
    EXPECT_NEAR(result[0], 0.25, 1e-10);
    EXPECT_NEAR(result[1], 0.75, 1e-10);
}

TEST(NaturalNeighbor, Performance) {
    // Create a larger dataset for performance testing
    const int grid_size = 20;
    df::Serie<Vector2> points;
    df::Serie<double> values;

    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            double x = i / double(grid_size - 1);
            double y = j / double(grid_size - 1);
            points.add({x, y});
            values.add(test_function(x, y));
        }
    }

    // Create many target points
    df::Serie<Vector2> targets;
    const int target_size = 1000;
    for (int i = 0; i < target_size; ++i) {
        double x = rand() / double(RAND_MAX);
        double y = rand() / double(RAND_MAX);
        targets.add({x, y});
    }

    // Measure interpolation time
    double elapsed = TIMING([&]() {
        auto result =
            df::natural_neighbor_2d(points, values, targets);
    });

    MSG("Interpolation of " << target_size << " points took " << elapsed
                            << " ms");

    // Should complete in reasonable time
    EXPECT_LT(elapsed, 5000.0); // 5 seconds max
}

TEST(NaturalNeighbor, Consistency) {
    // Test that interpolation is consistent with different point orderings
    df::Serie<Vector2> points1 = {
        {0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}};

    df::Serie<Vector2> points2 = {
        {1.0, 1.0}, {0.0, 1.0}, {1.0, 0.0}, {0.0, 0.0}};

    df::Serie<double> values1 = {0.0, 1.0, 1.0, 2.0};
    df::Serie<double> values2 = {2.0, 1.0, 1.0, 0.0};

    df::Serie<Vector2> targets = {{0.5, 0.5}};

    auto result1 =
        df::natural_neighbor_2d(points1, values1, targets);
    auto result2 =
        df::natural_neighbor_2d(points2, values2, targets);

    EXPECT_NEAR(result1[0], result2[0], 1e-10);
}

RUN_TESTS();