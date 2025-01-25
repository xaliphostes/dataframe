
#include "TEST.h"
#include <dataframe/functional/geo/distance_field.h>
#include <cmath>
#include <random>

// Test 2D distance field with single point
TEST(DistanceField, SinglePoint2D) {
    // Create single point at (0,0)
    std::vector<double> point_data = {0.0, 0.0};
    df::GenSerie<double> points(2, point_data);
    
    // Set up grid
    std::vector<double> bounds = {-1.0, 1.0, -1.0, 1.0};  // [-1,1] x [-1,1]
    std::vector<uint32_t> resolution = {5, 5};  // 5x5 grid
    
    MSG("Testing 2D distance field with single point");
    auto result = df::geo::distance_field(points, bounds, resolution);
    
    // Check grid points
    for (uint32_t i = 0; i < result.count(); ++i) {
        // Convert flat index to 2D coordinates
        uint32_t y = i / resolution[0];
        uint32_t x = i % resolution[0];
        
        // Compute expected coordinates
        double px = bounds[0] + x * (bounds[1] - bounds[0]) / (resolution[0] - 1);
        double py = bounds[2] + y * (bounds[3] - bounds[2]) / (resolution[1] - 1);
        
        // Expected distance
        double expected_dist = std::sqrt(px*px + py*py);
        EXPECT_NEAR(result.value(i), expected_dist, 1e-10);
    }
}

// Test 3D distance field with simple point set
TEST(DistanceField, SimplePoints3D) {
    // Create points at corners of a unit cube
    std::vector<double> point_data = {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        1.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        1.0, 0.0, 1.0,
        0.0, 1.0, 1.0,
        1.0, 1.0, 1.0
    };
    df::GenSerie<double> points(3, point_data);
    
    // Set up grid
    std::vector<double> bounds = {-0.5, 1.5, -0.5, 1.5, -0.5, 1.5};
    std::vector<uint32_t> resolution = {4, 4, 4};
    
    MSG("Testing 3D distance field with cube corners");
    auto result = df::geo::distance_field(points, bounds, resolution);
    
    // Test some known points
    // Center point should have distance to nearest corner
    uint32_t center_idx = 
        1 * (resolution[0] * resolution[1]) +  // z = 1
        1 * resolution[0] +                    // y = 1
        1;                                     // x = 1
    double expected_center_dist = std::sqrt(3) * 0.5;  // Distance to nearest corner
    EXPECT_NEAR(result.value(center_idx), expected_center_dist, 1e-2);
}

// Test signed distance field
TEST(DistanceField, SignedField) {
    // Create a circle of points with normals pointing outward
    std::vector<double> point_data;
    std::vector<double> normal_data;
    const int num_points = 16;
    const double radius = 1.0;
    
    for (int i = 0; i < num_points; ++i) {
        double angle = 2.0 * M_PI * i / num_points;
        double x = radius * std::cos(angle);
        double y = radius * std::sin(angle);
        
        point_data.push_back(x);
        point_data.push_back(y);
        
        // Normal points outward
        normal_data.push_back(std::cos(angle));
        normal_data.push_back(std::sin(angle));
    }
    
    df::GenSerie<double> points(2, point_data);
    df::GenSerie<double> normals(2, normal_data);
    
    // Set up grid
    std::vector<double> bounds = {-2.0, 2.0, -2.0, 2.0};
    std::vector<uint32_t> resolution = {9, 9};
    
    MSG("Testing signed distance field");
    auto result = df::geo::signed_distance_field(points, normals, bounds, resolution);
    
    // Test points inside and outside the circle
    // Center point should be negative (inside)
    uint32_t center_idx = (resolution[0] / 2) * resolution[0] + (resolution[0] / 2);
    EXPECT_TRUE(result.value(center_idx) < 0);
    
    // Corner point should be positive (outside)
    uint32_t corner_idx = 0;  // Top-left corner
    EXPECT_TRUE(result.value(corner_idx) > 0);
}

// Test error conditions
TEST(DistanceField, ErrorHandling) {
    MSG("Testing error handling");
    
    // Create some valid data
    std::vector<double> points_2d = {0.0, 0.0, 1.0, 1.0};
    std::vector<double> valid_bounds = {-1.0, 1.0, -1.0, 1.0};
    std::vector<uint32_t> valid_resolution = {5, 5};
    
    df::GenSerie<double> points(2, points_2d);
    
    // Test wrong dimension for bounds
    std::vector<double> wrong_bounds = {-1.0, 1.0, -1.0};  // Missing one bound
    EXPECT_THROW(
        df::geo::distance_field(points, wrong_bounds, valid_resolution),
        std::invalid_argument
    );
    
    // Test wrong dimension for resolution
    std::vector<uint32_t> wrong_resolution = {5};  // Only one dimension
    EXPECT_THROW(
        df::geo::distance_field(points, valid_bounds, wrong_resolution),
        std::invalid_argument
    );
    
    // Test wrong point dimension
    std::vector<double> points_4d = {0.0, 0.0, 0.0, 0.0};
    df::GenSerie<double> bad_points(4, points_4d);
    EXPECT_THROW(
        df::geo::distance_field(bad_points, valid_bounds, valid_resolution),
        std::invalid_argument
    );
    
    // Test signed distance field with mismatched normals
    std::vector<double> wrong_normals = {1.0, 0.0};  // Not enough normals
    df::GenSerie<double> normals(2, wrong_normals);
    EXPECT_THROW(
        df::geo::signed_distance_field(points, normals, valid_bounds, valid_resolution),
        std::invalid_argument
    );
}

// Test wrapper function
TEST(DistanceField, Wrapper) {
    MSG("Testing make_distance_field wrapper");
    
    // Create simple point set
    std::vector<double> points_2d = {0.0, 0.0, 1.0, 1.0};
    df::GenSerie<double> points(2, points_2d);
    
    // Set up grid parameters
    std::vector<double> bounds = {-1.0, 2.0, -1.0, 2.0};
    std::vector<uint32_t> resolution = {10, 10};
    
    // Create operator
    auto distance_op = df::geo::make_distance_field(bounds, resolution);
    auto result = distance_op(points);
    
    // Basic checks
    EXPECT_EQ(result.count(), resolution[0] * resolution[1]);
    EXPECT_EQ(result.itemSize(), 1);
    
    // Test signed field wrapper
    auto signed_op = df::geo::make_distance_field(bounds, resolution, true);
    
    // Should throw without normals
    EXPECT_THROW(
        signed_op(points),
        std::invalid_argument
    );
    
    // Should work with normals
    std::vector<double> normal_data = {1.0, 0.0, 0.0, 1.0};
    df::GenSerie<double> normals(2, normal_data);
    auto signed_result = signed_op(points, normals);
    
    EXPECT_EQ(signed_result.count(), resolution[0] * resolution[1]);
    EXPECT_EQ(signed_result.itemSize(), 1);
}

// Test with performance measures
TEST(DistanceField, Performance) {
    MSG("Testing performance with larger grid");
    
    // Create scattered points
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    
    const int num_points = 100;
    std::vector<double> point_data;
    for (int i = 0; i < num_points; ++i) {
        point_data.push_back(dis(gen));  // x
        point_data.push_back(dis(gen));  // y
        point_data.push_back(dis(gen));  // z
    }
    
    df::GenSerie<double> points(3, point_data);
    
    // Set up larger grid
    std::vector<double> bounds = {-0.5, 1.5, -0.5, 1.5, -0.5, 1.5};
    std::vector<uint32_t> resolution = {20, 20, 20};  // 8000 grid points
    
    MSG("Computing distance field for " + std::to_string(num_points) + 
        " points on " + std::to_string(resolution[0]) + "x" + 
        std::to_string(resolution[1]) + "x" + std::to_string(resolution[2]) + 
        " grid");
        
    auto result = df::geo::distance_field(points, bounds, resolution);
    
    // Verify basic properties
    EXPECT_EQ(result.count(), resolution[0] * resolution[1] * resolution[2]);
    EXPECT_EQ(result.itemSize(), 1);
    
    // Verify all distances are non-negative
    for (uint32_t i = 0; i < result.count(); ++i) {
        CHECK(result.value(i) >= 0.0);
    }
}

// Add main function to run all tests
RUN_TESTS()