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
#include <array>
#include <dataframe/math/bounds.h>

using namespace df;

TEST(Serie, BoundsArithmetic) {
    MSG("Testing bounds with arithmetic types");

    // Test with integers
    Serie<int> s1{1, -2, 3, -4, 5};

    auto [min_int, max_int] = bounds(s1);
    EXPECT_EQ(min_int, -4);
    EXPECT_EQ(max_int, 5);

    // Test individual min/max functions
    EXPECT_EQ(min(s1), -4);
    EXPECT_EQ(max(s1), 5);

    // Test with doubles
    Serie<double> s2{1.5, -2.5, 3.5, -4.5, 5.5};
    auto [min_double, max_double] = bounds(s2);
    EXPECT_NEAR(min_double, -4.5, 1e-10);
    EXPECT_NEAR(max_double, 5.5, 1e-10);

    // Test pipeline operations
    auto [min_pipe, max_pipe] = s1 | bind_bounds();
    EXPECT_EQ(min_pipe, -4);
    EXPECT_EQ(max_pipe, 5);

    auto min_pipe_val = s1 | bind_min();
    auto max_pipe_val = s1 | bind_max();
    EXPECT_EQ(min_pipe_val, -4);
    EXPECT_EQ(max_pipe_val, 5);
}

TEST(Serie, BoundsVector) {
    MSG("Testing bounds with Vector3D");

    using Vector3D = std::array<double, 3>;

    Serie<Vector3D> s1{Vector3D{1.0, -2.0, 3.0}, Vector3D{-4.0, 5.0, -6.0},
                       Vector3D{0.0, 1.0, -1.0}};

    // Test bounds
    auto [min_val, max_val] = bounds(s1);
    EXPECT_NEAR(min_val, -6.0, 1e-10);
    EXPECT_NEAR(max_val, 5.0, 1e-10);

    // Test individual min/max
    auto min_vector = min(s1);
    auto max_vector = max(s1);
    EXPECT_NEAR(min_vector, -6.0, 1e-10);
    EXPECT_NEAR(max_vector, 5.0, 1e-10);

    // Test pipeline operations
    auto [min_pipe, max_pipe] = s1 | bind_bounds();
    EXPECT_NEAR(min_pipe, -6.0, 1e-10);
    EXPECT_NEAR(max_pipe, 5.0, 1e-10);
}

TEST(Serie, BoundsEmpty) {
    MSG("Testing bounds with empty serie");

    Serie<double> empty_serie;
    EXPECT_THROW(bounds(empty_serie), std::runtime_error);
    EXPECT_THROW(min(empty_serie), std::runtime_error);
    EXPECT_THROW(max(empty_serie), std::runtime_error);
}

RUN_TESTS()