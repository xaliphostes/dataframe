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
#include <dataframe/map.h>
#include <dataframe/utils/range.h>

TEST(range, basic_range) {
    auto result = df::range(5);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[4] == 4);
}

TEST(range, start_end_range) {
    auto result = df::range(2, 6);
    EXPECT_TRUE(result.size() == 4);
    EXPECT_TRUE(result[0] == 2);
    EXPECT_TRUE(result[3] == 5);
}

TEST(range, step_range) {
    auto result = df::range(0, 10, 2);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[2] == 4);
    EXPECT_TRUE(result[4] == 8);
}

TEST(range, floating_point_range) {
    auto result = df::range(0.0, 1.0, 0.2);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_NEAR(result[0], 0.0, 1e-10);
    EXPECT_NEAR(result[2], 0.4, 1e-10);
    EXPECT_NEAR(result[4], 0.8, 1e-10);
}

TEST(range, negative_step) {
    auto result = df::range(5, 0, -1);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 5);
    EXPECT_TRUE(result[4] == 1);
}

TEST(range, zero_step_throws) {
    EXPECT_THROW(df::range(0, 5, 0), std::invalid_argument);
}

TEST(range, empty_range) {
    auto result = df::range(0, 0);
    EXPECT_TRUE(result.size() == 0);
}

// TEST(range, reverse_bounds_empty) {
//     auto result = df::range(5, 0); // Without negative step
//     df::print(result);
//     EXPECT_TRUE(result.size() == 0);
// }

TEST(range, pipe_operator) {
    auto result = df::range(0, 5) | df::bind_map([](int x, size_t) { return x * 2; });
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[2] == 4);
    EXPECT_TRUE(result[4] == 8);
}

TEST(range, bind_range) {
    auto range_gen = df::bind_range(0, 5);
    auto result = range_gen();
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[4] == 4);
}

TEST(range, type_deduction) {
    auto int_range = df::range(5);
    auto double_range = df::range(5.0);
    EXPECT_STREQ(int_range.type(), "int");
    EXPECT_STREQ(double_range.type(), "double");
}

// Test vector comparison using the helper function from TEST.h
TEST(range, vector_comparison) {
    auto result = df::range(3);
    std::vector<int> expected = {0, 1, 2};
    COMPARE_SERIE_VECTOR(result, expected);
}

RUN_TESTS();