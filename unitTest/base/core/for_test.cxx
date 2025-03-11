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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/for.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>

TEST(For, BasicLoop) {
    // Generate integers from 0 to 9
    auto result = df::for_loop<int>(0, 10, 1, [](int i) { return i; });

    EXPECT_EQ(result.size(), 10);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(result[i], i);
    }
}

TEST(For, StepLoop) {
    // Generate even numbers from 0 to 20
    auto result = df::for_loop<int>(0, 21, 2, [](int i) { return i; });

    EXPECT_EQ(result.size(), 11);
    for (size_t i = 0; i < 11; ++i) {
        EXPECT_EQ(result[i], i * 2);
    }
}

TEST(For, CustomFunction) {
    // Generate powers of 2
    auto result =
        df::for_loop<double>(0, 10, 1, [](int i) { return std::pow(2, i); });

    EXPECT_EQ(result.size(), 10);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(result[i], std::pow(2, i));
    }
}

TEST(For, NegativeStep) {
    // Generate a countdown from 10 to 1
    auto result = df::for_loop<int>(10, 0, -1, [](int i) { return i; });

    EXPECT_EQ(result.size(), 10);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(result[i], 10 - i);
    }
}

TEST(For, ErrorCases) {
    // Test with zero step
    EXPECT_THROW(df::for_loop<int>(0, 10, 0, [](int i) { return i; }),
                 std::invalid_argument);

    // Test with incompatible start/end/step
    EXPECT_THROW(df::for_loop<int>(0, 10, -1, [](int i) { return i; }),
                 std::invalid_argument);
    EXPECT_THROW(df::for_loop<int>(10, 0, 1, [](int i) { return i; }),
                 std::invalid_argument);
}

TEST(For, PipeUsage) {
    // Use for_loop in a pipe
    auto squares = df::bind_for_loop<int>(0, 10, 1)([](int i) { return i; }) |
                   df::bind_map([](int x, size_t) { return x * x; });

    EXPECT_EQ(squares.size(), 10);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(squares[i], i * i);
    }
}

TEST(For, EmptyResult) {
    // Test when start == end
    auto empty1 = df::for_loop<int>(5, 5, 1, [](int i) { return i; });
    EXPECT_EQ(empty1.size(), 0);
}

RUN_TESTS()