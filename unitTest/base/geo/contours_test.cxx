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
#include <dataframe/geo/mesh/contours.h>
#include <dataframe/geo/mesh/mesh.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(contours, single_triangle) {
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    Serie<double> values = {0.0, 1.0, 0.0};
    mesh.addVertexAttribute("test", values);

    auto segments = contours(mesh, "test", 0.5);
    EXPECT_EQ(segments.size(), 1);

    // Verify interpolated points
    auto seg1 = segments[0];
    EXPECT_NEAR(seg1.p1[0], 0.5, 1e-10);
    EXPECT_NEAR(seg1.p1[1], 0.0, 1e-10);
    EXPECT_NEAR(seg1.value, 0.5, 1e-10);
}

TEST(contours, square_mesh) {
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}, {0, 2, 3}};
    Mesh2D mesh(vertices, triangles);

    Serie<double> values = {0.0, 0.0, 1.0, 1.0};
    mesh.addVertexAttribute("test", values);

    auto segments = contours(mesh, "test", 0.5);
    EXPECT_EQ(segments.size(), 2);
}

TEST(contours, no_intersections) {
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    Serie<double> values = {0.0, 0.0, 0.0};
    mesh.addVertexAttribute("test", values);

    auto segments = contours(mesh, "test", 0.5);
    EXPECT_EQ(segments.size(), 0);
}

TEST(contours, error_handling) {
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    EXPECT_THROW(contours(mesh, "nonexistent", 0.5), std::runtime_error);

    Serie<Vector2> vectorValues = {{1, 0}, {0, 1}, {1, 1}};
    mesh.addVertexAttribute("vector", vectorValues);
    EXPECT_THROW(contours(mesh, "vector", 0.5), std::runtime_error);
}

TEST(contours, multiple_isoValues) {
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    Serie<double> values = {0.0, 1.0, 2.0};
    mesh.addVertexAttribute("test", values);

    std::vector<double> isoValues = {0.5, 1.5};
    auto segments = contours(mesh, "test", isoValues);
    df::print(segments);
    EXPECT_GT(segments.size(), 0);
}

/*
This test creates a 10x10 grid (200 triangles) with a radial scalar field
pattern and extracts 5 iso-contours. The test verifies segment endpoints are
within bounds and iso-values match the requested values.
*/
TEST(contours, grid_mesh) {
    // Create a 10x10 grid (200 triangles)
    const int nx = 10, ny = 10;
    Serie<Vector2> vertices;
    Triangles triangles;
    Serie<double> values;

    // Generate vertices and values
    for (int j = 0; j <= ny; ++j) {
        for (int i = 0; i <= nx; ++i) {
            double x = static_cast<double>(i) / nx;
            double y = static_cast<double>(j) / ny;
            vertices.add({x, y});

            // Create a radial pattern for the scalar field
            double cx = 0.5, cy = 0.5; // center
            double dx = x - cx, dy = y - cy;
            double value = std::sin(5.0 * std::sqrt(dx * dx + dy * dy));
            values.add(value);
        }
    }

    // df::print(values);

    // Generate triangles
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            uint v0 = j * (nx + 1) + i;
            uint v1 = v0 + 1;
            uint v2 = v0 + (nx + 1);
            uint v3 = v2 + 1;

            // Two triangles per grid cell
            triangles.add({v0, v1, v2});
            triangles.add({v1, v3, v2});
        }
    }

    Mesh2D mesh(vertices, triangles);
    mesh.addVertexAttribute("field", values);

    // Extract multiple iso-contours
    std::vector<double> isoValues = {-0.8, -0.4, 0.0, 0.4, 0.8};
    auto segments = contours(mesh, "field", isoValues);

    EXPECT_GT(segments.size(), 130);
    MSG("Generated ", segments.size(), " iso-segments");
    // df::print(segments);

    // Verify some properties
    for (size_t i = 0; i < segments.size(); ++i) {
        const auto &seg = segments[i];

        // Check that segment endpoints are within mesh bounds
        EXPECT_TRUE(seg.p1[0] >= 0.0 && seg.p1[0] <= 1.0);
        EXPECT_TRUE(seg.p1[1] >= 0.0 && seg.p1[1] <= 1.0);
        EXPECT_TRUE(seg.p2[0] >= 0.0 && seg.p2[0] <= 1.0);
        EXPECT_TRUE(seg.p2[1] >= 0.0 && seg.p2[1] <= 1.0);

        // Check iso-value is one of the requested values
        bool validIsoValue = false;
        for (double iso : isoValues) {
            if (std::abs(seg.value - iso) < 1e-10) {
                validIsoValue = true;
                break;
            }
        }
        EXPECT_TRUE(validIsoValue);
    }
}

TEST(iso_contours, different_generation_methods) {
    // Create test mesh and data
    Serie<Vector2> vertices = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1}, {0.5, 0.5} // Center vertex
    };
    Triangles triangles = {{0, 1, 4}, {1, 2, 4}, {2, 3, 4}, {3, 0, 4}};
    Mesh2D mesh(vertices, triangles);

    // Create radial scalar field
    Serie<double> values;
    for (const auto &v : vertices.data()) {
        double dx = v[0] - 0.5;
        double dy = v[1] - 0.5;
        values.add(std::sqrt(dx * dx + dy * dy));
    }
    mesh.addVertexAttribute("field", values);

    // Test 1: Fixed number of contours
    {
        auto isos = generateIsosByNumber(0.0, 0.7, 5);
        auto segments = contours(mesh, "field", isos);
        MSG("Generated ", segments.size(), " segments using fixed number");
        df::print(segments);
    }

    // Test 2: Fixed spacing
    {
        auto isos = generateIsosBySpacing(0.0, 0.7, 0.1);
        auto segments = contours(mesh, "field", isos);
        MSG("Generated ", segments.size(), " segments using fixed spacing");
        df::print(segments);
    }

    // Test 3: Using generateIsos with specific values
    {
        std::vector<double> specificValues = {0.2, 0.4, 0.6};
        auto isos = generateIsos(0.0, 0.7, specificValues);
        auto segments = contours(mesh, "field", isos);
        MSG("Generated ", segments.size(), " segments using specific values");
        df::print(segments);
    }

    // Test 4: Using generateIsos with spacing
    {
        auto isos = generateIsos(0.0, 0.7, {}, true, 0.15);
        auto segments = contours(mesh, "field", isos);
        MSG("Generated ", segments.size(),
            " segments using generateIsos with spacing");
        df::print(segments);
    }
}

RUN_TESTS()