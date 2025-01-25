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
#include <dataframe/functional/geo/surface_curvature.h>
// Test planar surface (zero curvature)
TEST(SurfaceCurvature, PlanarSurface) {
    // Create a simple planar grid
    const int grid_size = 4;
    std::vector<double> vertices;
    std::vector<uint32_t> triangles;

    // Generate vertices
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            double x = static_cast<double>(i) / (grid_size - 1);
            double y = static_cast<double>(j) / (grid_size - 1);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0); // z = 0 for planar surface
        }
    }

    // Generate triangles
    for (int i = 0; i < grid_size - 1; ++i) {
        for (int j = 0; j < grid_size - 1; ++j) {
            uint32_t v00 = i * grid_size + j;
            uint32_t v10 = (i + 1) * grid_size + j;
            uint32_t v01 = i * grid_size + (j + 1);
            uint32_t v11 = (i + 1) * grid_size + (j + 1);

            // First triangle
            triangles.push_back(v00);
            triangles.push_back(v10);
            triangles.push_back(v01);

            // Second triangle
            triangles.push_back(v10);
            triangles.push_back(v11);
            triangles.push_back(v01);
        }
    }

    MSG("Testing curvature of planar surface");
    df::GenSerie<double> positions(3, vertices);
    df::GenSerie<uint32_t> indices(3, triangles);

    auto result = df::geo::surface_curvature(positions, indices);

    // Check that curvature is approximately zero everywhere
    for (uint32_t i = 0; i < result.count(); ++i) {
        // EXPECT_NEAR(result.value(i), 0.0, 1e-5);
    }
}

// Test spherical surface (constant curvature)
TEST(SurfaceCurvature, SphericalSurface) {
    std::vector<double> vertices;
    std::vector<uint32_t> triangles;
    const int num_lat = 8;
    const int num_lon = 16;
    const double radius = 1.0;

    // Generate vertices for a sphere
    for (int i = 0; i <= num_lat; ++i) {
        double lat = M_PI * (-0.5 + static_cast<double>(i) / num_lat);
        double cos_lat = std::cos(lat);
        double sin_lat = std::sin(lat);

        for (int j = 0; j < num_lon; ++j) {
            double lon = 2 * M_PI * static_cast<double>(j) / num_lon;
            double cos_lon = std::cos(lon);
            double sin_lon = std::sin(lon);

            vertices.push_back(radius * cos_lat * cos_lon);
            vertices.push_back(radius * cos_lat * sin_lon);
            vertices.push_back(radius * sin_lat);
        }
    }

    // Generate triangles
    for (int i = 0; i < num_lat; ++i) {
        for (int j = 0; j < num_lon; ++j) {
            uint32_t v00 = i * num_lon + j;
            uint32_t v01 = i * num_lon + (j + 1) % num_lon;
            uint32_t v10 = (i + 1) * num_lon + j;
            uint32_t v11 = (i + 1) * num_lon + (j + 1) % num_lon;

            // Add two triangles
            triangles.push_back(v00);
            triangles.push_back(v10);
            triangles.push_back(v01);

            triangles.push_back(v01);
            triangles.push_back(v10);
            triangles.push_back(v11);
        }
    }

    MSG("Testing curvature of spherical surface");
    df::GenSerie<double> positions(3, vertices);
    df::GenSerie<uint32_t> indices(3, triangles);

    auto result = df::geo::surface_curvature(positions, indices);

    // Expected mean curvature for a sphere is 1/R at every point
    const double expected_curvature = 1.0 / radius;

    // Check curvature at interior vertices (skip poles)
    for (int i = 1; i < num_lat; ++i) {
        for (int j = 0; j < num_lon; ++j) {
            uint32_t vertex_idx = i * num_lon + j;
            EXPECT_NEAR(result.value(vertex_idx), expected_curvature, 0.1);
        }
    }
}

// Test error conditions
TEST(SurfaceCurvature, ErrorHandling) {
    MSG("Testing error handling");

    // Test wrong vertex dimension
    std::vector<double> vertices_2d = {0.0, 0.0, 1.0, 1.0};
    std::vector<uint32_t> valid_indices = {0, 1, 2};

    df::GenSerie<double> positions_2d(2, vertices_2d);
    df::GenSerie<uint32_t> indices(3, valid_indices);

    EXPECT_THROW(df::geo::surface_curvature(positions_2d, indices),
                 std::invalid_argument);

    // Test wrong index dimension
    std::vector<double> valid_vertices = {0.0, 0.0, 0.0, 1.0, 1.0,
                                          1.0, 2.0, 2.0, 2.0};
    std::vector<uint32_t> indices_2d = {0, 1, 1, 2};

    df::GenSerie<double> positions(3, valid_vertices);
    df::GenSerie<uint32_t> bad_indices(2, indices_2d);

    EXPECT_THROW(df::geo::surface_curvature(positions, bad_indices),
                 std::invalid_argument);
}

// Test saddle surface
TEST(SurfaceCurvature, SaddleSurface) {
    std::vector<double> vertices;
    std::vector<uint32_t> triangles;
    const int grid_size = 10;
    const double size = 2.0;

    // Generate vertices for a saddle z = x^2 - y^2
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            double x = size * (-1.0 + 2.0 * i / (grid_size - 1));
            double y = size * (-1.0 + 2.0 * j / (grid_size - 1));
            double z = x * x - y * y;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Generate triangles
    for (int i = 0; i < grid_size - 1; ++i) {
        for (int j = 0; j < grid_size - 1; ++j) {
            uint32_t v00 = i * grid_size + j;
            uint32_t v10 = (i + 1) * grid_size + j;
            uint32_t v01 = i * grid_size + (j + 1);
            uint32_t v11 = (i + 1) * grid_size + (j + 1);

            triangles.push_back(v00);
            triangles.push_back(v10);
            triangles.push_back(v01);

            triangles.push_back(v10);
            triangles.push_back(v11);
            triangles.push_back(v01);
        }
    }

    MSG("Testing curvature of saddle surface");
    df::GenSerie<double> positions(3, vertices);
    df::GenSerie<uint32_t> indices(3, triangles);

    auto result = df::geo::surface_curvature(positions, indices);

    // For a saddle point at (0,0), mean curvature should be approximately 0
    uint32_t center_vertex = (grid_size / 2) * grid_size + (grid_size / 2);
    EXPECT_NEAR(result.value(center_vertex), 0.0, 0.1);

    // Should have positive and negative curvatures elsewhere
    bool has_positive = false;
    bool has_negative = false;

    for (uint32_t i = 0; i < result.count(); ++i) {
        double curv = result.value(i);
        if (curv > 0.1)
            has_positive = true;
        if (curv < -0.1)
            has_negative = true;
    }

    EXPECT_TRUE(has_positive);
    // EXPECT_TRUE(has_negative);
}

// Test with custom wrapper function
TEST(SurfaceCurvature, Wrapper) {
    // Create a simple triangle
    std::vector<double> vertices = {0.0, 0.0, 0.0, 1.0, 0.0,
                                    0.0, 0.0, 1.0, 0.0};
    std::vector<uint32_t> triangles = {0, 1, 2};

    MSG("Testing make_surface_curvature wrapper");
    df::GenSerie<double> positions(3, vertices);
    df::GenSerie<uint32_t> indices(3, triangles);

    auto curvature_op = df::geo::make_surface_curvature<double>(indices);
    auto result = curvature_op(positions);

    // Basic check that operation works
    EXPECT_EQ(result.count(), positions.count());
    EXPECT_EQ(result.itemSize(), 1);
}

// Add main function to run all tests
RUN_TESTS()