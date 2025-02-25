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
#include <dataframe/geo/normals.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(normals, segments_2d) {
    MSG("Testing 2D segment normals");

    // Unit square
    Serie<Vector2> vertices{
        {0.0, 0.0}, // v0
        {1.0, 0.0}, // v1
        {1.0, 1.0}, // v2
        {0.0, 1.0}  // v3
    };

    Serie<iVector2> segments{
        {0, 1}, // bottom
        {1, 2}, // right
        {2, 3}, // top
        {3, 0}  // left
    };

    auto result = normals<double>(vertices, segments);
    EXPECT_EQ(result.size(), 4);

    // Expected normals for counter-clockwise square

    // bottom edge normal
    EXPECT_ARRAY_NEAR(result[0], Vector2({0.0, 1.0}), 1e-10);
    // right edge normal
    EXPECT_ARRAY_NEAR(result[1], Vector2({-1.0, 0.0}), 1e-10);
    // top edge normal
    EXPECT_ARRAY_NEAR(result[2], Vector2({0.0, -1.0}), 1e-10);
    // left edge normal
    EXPECT_ARRAY_NEAR(result[3], Vector2({1.0, 0.0}), 1e-10);

    // Test pipeline operation
    auto pipe_result = vertices | bind_normals<double>(segments);
    EXPECT_ARRAY_NEAR(pipe_result[0], result[0], 1e-10);
}

TEST(normals, triangles_3d) {
    MSG("Testing 3D triangle normals");

    // Simple tetrahedron
    Serie<Vector3> vertices{
        {0.0, 0.0, 0.0}, // v0
        {1.0, 0.0, 0.0}, // v1
        {0.5, 1.0, 0.0}, // v2
        {0.5, 0.5, 1.0}  // v3
    };

    Serie<iVector3> triangles{
        {0, 1, 2}, // base
        {0, 1, 3}, // front
        {1, 2, 3}, // right
        {2, 0, 3}  // left
    };

    auto result = normals<double>(vertices, triangles);
    EXPECT_EQ(result.size(), 4);
    df::print(result);

    // Verify all normals are unit length
    for (size_t i = 0; i < result.size(); ++i) {
        double length = std::sqrt(result[i][0] * result[i][0] +
                                  result[i][1] * result[i][1] +
                                  result[i][2] * result[i][2]);
        EXPECT_NEAR(length, 1.0, 1e-10);
    }

    // Base triangle should point down (-z direction)
    // EXPECT_NEAR(result[0][2], -1.0, 1e-10);

    // Test pipeline operation
    auto pipe_result = vertices | bind_normals<double>(triangles);
    EXPECT_ARRAY_NEAR(pipe_result[0], result[0], 1e-10);
}

TEST(normals, error_cases) {
    MSG("Testing normal calculation error cases");

    // Empty series
    Serie<Vector2> empty_vertices{};
    Serie<iVector2> empty_segments{};
    auto empty_result = normals<double>(empty_vertices, empty_segments);
    EXPECT_EQ(empty_result.size(), 0);

    // Zero-length segment case
    Serie<Vector2> degen_vertices{{0.0, 0.0}, {0.0, 0.0}};
    Serie<iVector2> degen_segments{{0, 1}};
    EXPECT_THROW(normals<double>(degen_vertices, degen_segments),
                 std::runtime_error);

    // Degenerate triangle case (colinear points)
    Serie<Vector3> degen_vertices3d{
        {0.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {2.0, 0.0, 0.0}};
    Serie<iVector3> degen_triangles{{0, 1, 2}};
    EXPECT_THROW(normals<double>(degen_vertices3d, degen_triangles),
                 std::runtime_error);
}

RUN_TESTS()