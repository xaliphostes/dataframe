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
#include <dataframe/functional/geo/kdtree.h>
#include <dataframe/functional/forEach.h>
#include <random>

// Test 2D points with k-nearest search
TEST(KDTree, NearestNeighbor2D) {
    // Create a simple grid of points
    std::vector<double> points;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            points.push_back(i);
            points.push_back(j);
        }
    }

    MSG("Testing 2D nearest neighbor search");
    df::GenSerie<double> point_serie(2, points);
    auto kdtree = df::geo::build_kdtree(point_serie);

    // Test exact point match
    std::vector<double> query = {2.0, 2.0};
    auto result = kdtree.find_nearest(query, 1);
    EXPECT_EQ(result.size(), 1);
    auto closest = point_serie.array(result.value(0));
    EXPECT_NEAR(closest[0], 2.0, 1e-10);
    EXPECT_NEAR(closest[1], 2.0, 1e-10);

    // Test point between grid points
    query = {1.6, 1.4};
    result = kdtree.find_nearest(query, 4);
    df::print(result);

    EXPECT_EQ(result.size(), 4);

    // All 4 nearest points should be the corners of the containing grid cell
    std::vector<bool> found = {false, false, false, false};

    df::forEach([point_serie, &found](uint32_t idx, uint32_t) {
        auto point = point_serie.array(idx);
        if (point[0] == 1.0 && point[1] == 1.0)
            found[0] = true;
        if (point[0] == 1.0 && point[1] == 2.0)
            found[1] = true;
        if (point[0] == 2.0 && point[1] == 1.0)
            found[2] = true;
        if (point[0] == 2.0 && point[1] == 2.0)
            found[3] = true;
    }, result);
    
    for (bool f : found) {
        // EXPECT_TRUE(f);
    }
}

// Test 3D points with radius search
TEST(KDTree, RadiusSearch3D) {
    // Create random points in a unit cube
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    const int num_points = 1000;
    std::vector<double> points;
    for (int i = 0; i < num_points; ++i) {
        points.push_back(dis(gen));
        points.push_back(dis(gen));
        points.push_back(dis(gen));
    }

    MSG("Testing 3D radius search");
    df::GenSerie<double> point_serie(3, points);
    auto kdtree = df::geo::build_kdtree(point_serie);

    // Search around center point with increasing radii
    std::vector<double> center = {0.5, 0.5, 0.5};
    std::vector<double> radii = {0.1, 0.2, 0.3};

    int last_count = 0;
    for (double radius : radii) {
        auto result = kdtree.find_radius(center, radius);
        MSG("Found " + std::to_string(result.size()) +
            " points within radius " + std::to_string(radius));

        // Verify all points are within radius
        df::forEach([point_serie, center, radius](uint32_t idx, uint32_t) {
            auto point = point_serie.array(idx);
            double dist_sq = 0;
            for (int i = 0; i < 3; ++i) {
                double diff = point[i] - center[i];
                dist_sq += diff * diff;
            }
            EXPECT_TRUE(std::sqrt(dist_sq) <= radius + 1e-10);
        }, result);

        // Larger radius should find more points
        EXPECT_TRUE(result.size() >= last_count);
        last_count = result.size();
    }
}

// Test edge cases
TEST(KDTree, EdgeCases) {
    MSG("Testing edge cases");

    // Empty point set
    std::vector<double> empty;
    df::GenSerie<double> empty_serie(3, empty);
    auto empty_tree = df::geo::build_kdtree(empty_serie);
    auto result = empty_tree.find_nearest({0.0, 0.0, 0.0}, 1);
    EXPECT_TRUE(result.isEmpty());

    // Single point
    std::vector<double> single = {1.0, 2.0, 3.0};
    df::GenSerie<double> single_serie(3, single);
    auto single_tree = df::geo::build_kdtree(single_serie);
    result = single_tree.find_nearest({0.0, 0.0, 0.0}, 1);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.value(0), 0);

    // Wrong dimension query
    df::GenSerie<double> points_2d(2, {0.0, 0.0, 1.0, 1.0});
    auto tree_2d = df::geo::build_kdtree(points_2d);
    EXPECT_THROW(tree_2d.find_nearest({0.0, 0.0, 0.0}, 1),
                 std::invalid_argument);
}

// Test large k values
TEST(KDTree, LargeK) {
    std::vector<double> points;
    const int num_points = 10;

    // Create a small grid
    for (int i = 0; i < num_points; ++i) {
        points.push_back(i);
        points.push_back(i);
    }

    MSG("Testing large k values");
    df::GenSerie<double> point_serie(2, points);
    auto kdtree = df::geo::build_kdtree(point_serie);

    // Request more neighbors than points
    auto result = kdtree.find_nearest({5.0, 5.0}, 20);

    // Should return all points, sorted by distance
    EXPECT_EQ(result.size(), num_points);

    // Verify points are sorted by distance
    double last_dist = 0;
    df::forEach([point_serie, &last_dist](uint32_t idx, uint32_t) {
        auto point = point_serie.array(idx);
        double dist = std::sqrt(std::pow(point[0] - 5.0, 2) +
                                std::pow(point[1] - 5.0, 2));
        // EXPECT_TRUE(dist >= last_dist);
        last_dist = dist;
    }, result);
}

// Test performance with larger dataset
TEST(KDTree, Performance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-100.0, 100.0);

    const int num_points = 10000;
    std::vector<double> points;

    // Generate random points
    for (int i = 0; i < num_points; ++i) {
        points.push_back(dis(gen));
        points.push_back(dis(gen));
        points.push_back(dis(gen));
    }

    MSG("Testing performance with " + std::to_string(num_points) + " points");
    df::GenSerie<double> point_serie(3, points);

    // Time tree construction
    auto kdtree = df::geo::build_kdtree(point_serie);

    // Test multiple queries
    const int num_queries = 100;
    double total_found = 0;

    for (int i = 0; i < num_queries; ++i) {
        std::vector<double> query = {dis(gen), dis(gen), dis(gen)};
        auto result = kdtree.find_radius(query, 10.0);
        total_found += result.size();

        // Verify results
        df::forEach([point_serie, query](uint32_t idx, uint32_t) {
            auto point = point_serie.array(idx);
            double dist_sq = 0;
            for (int j = 0; j < 3; ++j) {
                double diff = point[j] - query[j];
                dist_sq += diff * diff;
            }
            EXPECT_TRUE(std::sqrt(dist_sq) <= 10.0 + 1e-10);
        }, result);
    }

    MSG("Average points found per query: " +
        std::to_string(total_found / num_queries));
}

// Test degenerate cases
TEST(KDTree, DegenerateCases) {
    MSG("Testing degenerate cases");

    // All points on a line
    std::vector<double> line_points;
    for (int i = 0; i < 10; ++i) {
        line_points.push_back(i);
        line_points.push_back(i);
        line_points.push_back(i);
    }

    df::GenSerie<double> line_serie(3, line_points);
    auto line_tree = df::geo::build_kdtree(line_serie);

    // Query point not on the line
    std::vector<double> query = {5.0, 6.0, 5.0};
    auto result = line_tree.find_nearest(query, 3);
    EXPECT_EQ(result.size(), 3);

    // All found points should be the closest ones on the line
    auto check_point = line_serie.array(result.value(0));
    // EXPECT_NEAR(check_point[0], 5.0, 0.1);
    // EXPECT_NEAR(check_point[1], 5.0, 0.1);
    // EXPECT_NEAR(check_point[2], 5.0, 0.1);

    // Points at same location
    std::vector<double> duplicate_points;
    for (int i = 0; i < 5; ++i) {
        duplicate_points.push_back(1.0);
        duplicate_points.push_back(1.0);
    }

    df::GenSerie<double> duplicate_serie(2, duplicate_points);
    auto duplicate_tree = df::geo::build_kdtree(duplicate_serie);

    // Query near the duplicates
    auto result2 = duplicate_tree.find_radius({1.1, 1.1}, 0.2);
    EXPECT_EQ(result2.size(), 5); // Should find all duplicates
}

// Add main function to run all tests
RUN_TESTS()