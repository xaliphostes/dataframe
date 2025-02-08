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
#include <dataframe/geo/curvature.h>
#include <dataframe/geo/mesh/mesh.h>

using namespace df;

// Helper functions for test verification
namespace {

// Create unit sphere vertices and triangles for known curvature test
std::pair<Positions3, Triangles> create_unit_sphere(int refinement = 1) {
    // Start with octahedron vertices
    Positions3 vertices = {{0, 0, 1},  {0, 0, -1}, {1, 0, 0},
                           {-1, 0, 0}, {0, 1, 0},  {0, -1, 0}};

    Triangles triangles = {{0, 4, 2}, {0, 2, 5}, {0, 5, 3}, {0, 3, 4},
                           {1, 2, 4}, {1, 5, 2}, {1, 3, 5}, {1, 4, 3}};

    // Refine mesh by subdividing triangles
    for (int r = 0; r < refinement; ++r) {
        Triangles new_triangles;
        std::map<std::pair<size_t, size_t>, size_t> edge_vertices;

        triangles.forEach([&](const iVector3 &tri, size_t) {
            // for (const auto &tri : triangles) {
            // Create new vertices at edge midpoints
            std::array<uint, 3> mids;
            for (int i = 0; i < 3; ++i) {
                size_t v1 = tri[i];
                size_t v2 = tri[(i + 1) % 3];
                auto edge = std::make_pair(std::min(v1, v2), std::max(v1, v2));

                if (edge_vertices.find(edge) == edge_vertices.end()) {
                    Vector3 mid = {(vertices[v1][0] + vertices[v2][0]) / 2,
                                   (vertices[v1][1] + vertices[v2][1]) / 2,
                                   (vertices[v1][2] + vertices[v2][2]) / 2};
                    // Project to unit sphere
                    double len = std::sqrt(mid[0] * mid[0] + mid[1] * mid[1] +
                                           mid[2] * mid[2]);
                    mid = {mid[0] / len, mid[1] / len, mid[2] / len};

                    edge_vertices[edge] = vertices.size();
                    vertices.add(mid);
                }
                mids[i] = edge_vertices[edge];
            }

            // Create four new triangles
            new_triangles.add({tri[0], mids[0], mids[2]});
            new_triangles.add({mids[0], tri[1], mids[1]});
            new_triangles.add({mids[2], mids[1], tri[2]});
            new_triangles.add({mids[0], mids[1], mids[2]});
        });
        triangles = new_triangles;
    }

    return {vertices, triangles};
}

} // anonymous namespace

TEST(curvature, sphere_curvature) {
    MSG("Testing curvature computation on unit sphere");

    // Create unit sphere mesh
    auto [vertices, triangles] = create_unit_sphere(2); // 2 refinements

    // Compute curvatures
    auto results = surface_curvature(vertices, triangles);

    // For unit sphere, mean curvature should be 1.0 and gaussian curvature
    // should be 1.0
    const auto &mean_curv = results.get<double>("mean_curvature");
    const auto &gauss_curv = results.get<double>("gaussian_curvature");
    const auto &k1 = results.get<double>("k1");
    const auto &k2 = results.get<double>("k2");

    df::print(mean_curv);
    df::print(gauss_curv);
    df::print(k1);
    df::print(k2);

    for (size_t i = 0; i < vertices.size(); ++i) {
        EXPECT_NEAR(mean_curv[i], 1.0, 0.1); // Allow some discretization error
        EXPECT_NEAR(gauss_curv[i], 1.0, 0.1);
        EXPECT_NEAR(k1[i], 1.0, 0.1);
        EXPECT_NEAR(k2[i], 1.0, 0.1);
    }
}

TEST(curvature, tensor_properties) {
    MSG("Testing curvature tensor properties");

    auto [vertices, triangles] = create_unit_sphere(2);
    auto results = surface_curvature(vertices, triangles);

    const auto &tensors =
        results.get<std::array<double, 9>>("curvature_tensor");
    const auto &dir1 = results.get<Vector3>("principal_direction1");
    const auto &dir2 = results.get<Vector3>("principal_direction2");

    for (size_t i = 0; i < vertices.size(); ++i) {
        // Test tensor symmetry
        EXPECT_TRUE(is_symmetric(tensors[i]));

        // Test principal directions are unit vectors
        EXPECT_NEAR(length(dir1[i]), 1.0, 1e-10);
        EXPECT_NEAR(length(dir2[i]), 1.0, 1e-10);

        // Test principal directions are orthogonal
        EXPECT_TRUE(is_orthogonal(dir1[i], dir2[i]));

        // Test principal directions are orthogonal to normal
        Vector3 normal = vertices[i]; // For unit sphere, position = normal
        EXPECT_TRUE(is_orthogonal(dir1[i], normal));
        EXPECT_TRUE(is_orthogonal(dir2[i], normal));
    }
}

TEST(curvature, saddle_point) {
    MSG("Testing curvature at saddle point");

    // Create a simple saddle surface z = x^2 - y^2
    Positions3 vertices;
    double size = 1.0;
    int n = 5;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double x = size * (-1.0 + 2.0 * i / (n - 1));
            double y = size * (-1.0 + 2.0 * j / (n - 1));
            double z = x * x - y * y;
            vertices.add({x, y, z});
        }
    }

    // Create triangles
    Triangles triangles;
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1; ++j) {
            uint v00 = i * n + j;
            uint v10 = (i + 1) * n + j;
            uint v01 = i * n + (j + 1);
            uint v11 = (i + 1) * n + (j + 1);
            triangles.add({v00, v10, v11});
            triangles.add({v00, v11, v01});
        }
    }

    auto results = surface_curvature(vertices, triangles);

    // Test curvature at central point
    size_t center = (n * n) / 2;
    const auto &k1 = results.get<double>("k1");
    const auto &k2 = results.get<double>("k2");

    // At origin, principal curvatures should be approximately +2 and -2
    EXPECT_NEAR(k1[center], 2.0, 0.5);
    EXPECT_NEAR(k2[center], -2.0, 0.5);

    // Mean curvature should be near 0 at origin
    const auto &mean_curv = results.get<double>("mean_curvature");
    EXPECT_NEAR(mean_curv[center], 0.0, 0.1);
}

TEST(curvature, error_cases) {
    MSG("Testing error handling in curvature computation");

    // Empty mesh
    Positions3 empty_verts;
    Triangles empty_tris;
    auto results = surface_curvature(empty_verts, empty_tris);
    EXPECT_EQ(results.get<double>("mean_curvature").size(), 0);

    // Single triangle (boundary case)
    Positions3 single_verts = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
    Triangles single_tri = {{0, 1, 2}};
    auto single_results = surface_curvature(single_verts, single_tri);
    EXPECT_EQ(single_results.get<double>("mean_curvature").size(), 3);

    // Invalid triangle indices
    Positions3 verts = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
    Triangles invalid_tris = {{0, 1, 3}}; // 3 is out of bounds
    EXPECT_THROW(surface_curvature(verts, invalid_tris), std::out_of_range);
}

RUN_TESTS()