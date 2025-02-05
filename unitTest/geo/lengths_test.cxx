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
#include <dataframe/geo/length.h>
#include <dataframe/pipe.h>

using namespace df;

TEST(metrics, segment_length_2d) {
    MSG("Testing 2D segment length calculations");

    // Unit square
    Serie<Vector2> vertices{
        {0.0, 0.0}, // v0
        {1.0, 0.0}, // v1
        {1.0, 1.0}, // v2
        {0.0, 1.0}  // v3
    };

    Serie<std::array<uint, 2>> segments{
        {0, 1}, // bottom (length 1)
        {1, 2}, // right (length 1)
        {2, 3}, // top (length 1)
        {3, 0}, // left (length 1)
        {0, 2}  // diagonal (length √2)
    };

    auto result = length<2>(vertices, segments);
    EXPECT_EQ(result.size(), 5);

    // Check lengths
    EXPECT_NEAR(result[0], 1.0, 1e-10);
    EXPECT_NEAR(result[1], 1.0, 1e-10);
    EXPECT_NEAR(result[2], 1.0, 1e-10);
    EXPECT_NEAR(result[3], 1.0, 1e-10);
    EXPECT_NEAR(result[4], std::sqrt(2.0), 1e-10);

    // Test pipeline
    auto pipe_result = vertices | bind_length<2>(segments);
    EXPECT_ARRAY_NEAR(pipe_result.asArray(), result.asArray(), 1e-10);
}

TEST(metrics, segment_length_3d) {
    MSG("Testing 3D segment length calculations");

    // Unit cube
    Serie<Vector3> vertices{
        {0.0, 0.0, 0.0}, // v0
        {1.0, 0.0, 0.0}, // v1
        {1.0, 1.0, 0.0}, // v2
        {0.0, 1.0, 0.0}, // v3
        {0.0, 0.0, 1.0}, // v4
        {1.0, 0.0, 1.0}, // v5
        {1.0, 1.0, 1.0}, // v6
        {0.0, 1.0, 1.0}  // v7
    };

    Serie<std::array<uint, 2>> segments{
        {0, 1}, // edge (length 1)
        {0, 4}, // edge (length 1)
        {0, 6}  // diagonal (length √3)
    };

    auto result = length<3>(vertices, segments);
    EXPECT_EQ(result.size(), 3);

    // Check lengths
    EXPECT_NEAR(result[0], 1.0, 1e-10);
    EXPECT_NEAR(result[1], 1.0, 1e-10);
    EXPECT_NEAR(result[2], std::sqrt(3.0), 1e-10);
}

TEST(metrics, error_cases) {
    MSG("Testing metric calculation error cases");

    // Empty series
    Serie<Vector2> empty_vertices{};
    Serie<std::array<uint, 2>> empty_segments{};

    auto empty_lengths = length<2>(empty_vertices, empty_segments);
    EXPECT_EQ(empty_lengths.size(), 0);
}

RUN_TESTS()