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
 */

#include "TEST.h"
#include <cmath>
#include <dataframe/Dataframe.h>

/**
 * @brief Example showing how to create store a triangulated surface mesh using
 * the Dataframe and 2 series.
 *
 * @code
 * Copy// Create a mesh
 * auto mesh = createWavySurface(10, 10);
 *
 * // Access vertex positions
 * const auto& positions = mesh.get<Position>("positions");
 * const auto& triangles = mesh.get<Triangle>("triangles");
 *
 * // Example: Calculate the center of each triangle
 * positions.forEach([&](const Position& pos, uint32_t idx) {
 *     // Process each vertex
 * });
 *
 * triangles.forEach([&](const Triangle& tri, uint32_t idx) {
 *     // Process each triangle
 *     const Position& v0 = positions[tri[0]];
 *     const Position& v1 = positions[tri[1]];
 *     const Position& v2 = positions[tri[2]];
 *     // ... do something with the triangle vertices
 * });
 * @endcode
 */

using Position = std::array<double, 3>;
using Triangle = std::array<uint32_t, 3>;

// Create a wavy triangulated surface
df::DataFrame createWavySurface(int nx = 5, int ny = 5, double width = 10.0,
                                double length = 10.0, double amplitude = 2.0,
                                double frequency = 0.5) {

    df::DataFrame mesh;
    std::vector<Position> positions;
    std::vector<Triangle> triangles;

    // Generate vertex positions in a grid
    double dx = width / (nx - 1);
    double dy = length / (ny - 1);

    // Create vertices
    for (int j = 0; j < ny; ++j) {
        for (int i = 0; i < nx; ++i) {
            double x = i * dx - width / 2;
            double y = j * dy - length / 2;

            // Create a wavy surface using sine functions
            double z =
                amplitude * std::sin(frequency * x) * std::cos(frequency * y);

            positions.push_back({x, y, z});
        }
    }

    // Generate triangles
    for (int j = 0; j < ny - 1; ++j) {
        for (int i = 0; i < nx - 1; ++i) {
            // Get indices for the current quad
            uint32_t v0 = j * nx + i;
            uint32_t v1 = v0 + 1;
            uint32_t v2 = (j + 1) * nx + i;
            uint32_t v3 = v2 + 1;

            // Create two triangles for each quad
            triangles.push_back({v0, v1, v2}); // First triangle
            triangles.push_back({v1, v3, v2}); // Second triangle
        }
    }

    // --------------------------------------

    mesh.add("positions", positions);
    mesh.add("triangles", triangles);
    return mesh;
}

TEST(MeshExample, creation) {
    // Create a 10x10 grid which will generate 200 triangles
    df::DataFrame mesh = createWavySurface(10, 10);

    // Verify the mesh
    EXPECT_TRUE(mesh.has("positions"));
    EXPECT_TRUE(mesh.has("triangles"));

    const auto &positions = mesh.get<Position>("positions");
    const auto &triangles = mesh.get<Triangle>("triangles");

    // A 10x10 grid should have 100 vertices
    EXPECT_EQ(positions.size(), 100);

    // Number of triangles should be 2 * (9 * 9) = 162
    // because each grid cell creates 2 triangles
    EXPECT_EQ(triangles.size(), 162);

    // Print some statistics
    MSG("Mesh statistics:");
    MSG("  Number of vertices: " << positions.size());
    MSG("  Number of triangles: " << triangles.size());

    // Verify some mesh properties
    // Check that triangle indices are within bounds
    bool valid_indices = true;
    triangles.forEach([&](const Triangle &tri, uint32_t) {
        for (uint32_t idx : tri) {
            if (idx >= positions.size()) {
                valid_indices = false;
            }
        }
    });
    EXPECT_TRUE(valid_indices);

    // Calculate and print mesh bounds
    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = min_x;
    double max_y = max_x;
    double min_z = min_x;
    double max_z = max_x;

    positions.forEach([&](const Position &pos, uint32_t) {
        min_x = std::min(min_x, pos[0]);
        max_x = std::max(max_x, pos[0]);
        min_y = std::min(min_y, pos[1]);
        max_y = std::max(max_y, pos[1]);
        min_z = std::min(min_z, pos[2]);
        max_z = std::max(max_z, pos[2]);
    });

    MSG("Mesh bounds:");
    MSG("  X: [" << min_x << ", " << max_x << "]");
    MSG("  Y: [" << min_y << ", " << max_y << "]");
    MSG("  Z: [" << min_z << ", " << max_z << "]");

    // Example of using map to transform vertices
    auto scaled_positions = positions.map([](const Position &pos, uint32_t) {
        return Position{pos[0] * 2.0, pos[1] * 2.0, pos[2] * 2.0};
    });
    mesh.add("scaled_positions", scaled_positions);
    EXPECT_TRUE(mesh.has("scaled_positions"));
}

TEST(MeshExample, manipulation) {
    // Create a smaller mesh for testing transformations
    df::DataFrame mesh = createWavySurface(5, 5);
    const auto &positions = mesh.get<Position>("positions");

    // Create a transformed copy of the positions using map
    auto transformed = positions.map([](const Position &pos, uint32_t) {
        // Rotate around Y axis by 45 degrees and translate
        double angle = M_PI / 4.0;
        double x = pos[0] * std::cos(angle) - pos[2] * std::sin(angle) + 1.0;
        double y = pos[1] + 2.0;
        double z = pos[0] * std::sin(angle) + pos[2] * std::cos(angle) + 3.0;
        return Position{x, y, z};
    });

    mesh.add("transformed_positions", transformed);
    EXPECT_EQ(transformed.size(), positions.size());
}

RUN_TESTS();