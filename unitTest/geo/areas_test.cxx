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
#include <dataframe/geo/area.h>
#include <dataframe/pipe.h>

using namespace df;

TEST(metrics, triangle_area_2d) {
    MSG("Testing 2D triangle area calculations");

    Serie<Vector2> vertices{
        {0.0, 0.0}, // v0
        {1.0, 0.0}, // v1
        {0.0, 1.0}, // v2
        {1.0, 1.0}  // v3
    };

    Serie<std::array<uint, 3>> triangles{
        {0, 1, 2}, // right triangle (area 0.5)
        {1, 3, 2}  // right triangle (area 0.5)
    };

    auto result = area(vertices, triangles);
    EXPECT_EQ(result.size(), 2);

    // Check areas
    EXPECT_NEAR(result[0], 0.5, 1e-10);
    EXPECT_NEAR(result[1], 0.5, 1e-10);

    // Test pipeline
    auto pipe_result = vertices | bind_area(triangles);
    EXPECT_ARRAY_NEAR(pipe_result.asArray(), result.asArray(), 1e-10);
}

TEST(metrics, triangle_area_3d) {
    MSG("Testing 3D triangle area calculations");

    Serie<Vector3> vertices{
        {0.0, 0.0, 0.0}, // v0
        {1.0, 0.0, 0.0}, // v1
        {0.0, 1.0, 0.0}, // v2
        {0.0, 0.0, 1.0}  // v3
    };

    Serie<std::array<uint, 3>> triangles{
        {0, 1, 2}, // triangle in xy plane (area 0.5)
        {0, 1, 3}, // triangle in xz plane (area 0.5)
        {0, 2, 3}  // triangle in yz plane (area 0.5)
    };

    auto result = area(vertices, triangles);
    EXPECT_EQ(result.size(), 3);

    // Check areas
    EXPECT_NEAR(result[0], 0.5, 1e-10);
    EXPECT_NEAR(result[1], 0.5, 1e-10);
    EXPECT_NEAR(result[2], 0.5, 1e-10);
}

TEST(metrics, error_cases) {
    MSG("Testing metric calculation error cases");

    // Empty series
    Serie<Vector2> empty_vertices{};
    Serie<std::array<uint, 3>> empty_triangles{};

    auto empty_areas = area(empty_vertices, empty_triangles);
    EXPECT_EQ(empty_areas.size(), 0);
}

RUN_TESTS()