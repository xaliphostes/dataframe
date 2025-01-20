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
#include <dataframe/functional/utils/slice.h>

TEST(slice, scalar_serie) {
    // Test scalar series
    df::GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6});

    auto result = df::utils::slice(1, 4, s1);
    EXPECT_EQ(result.count(), 3);
    EXPECT_EQ(result.itemSize(), 1);
    EXPECT_NEAR(result.value(0), 2.0, 1e-10);
    EXPECT_NEAR(result.value(1), 3.0, 1e-10);
    EXPECT_NEAR(result.value(2), 4.0, 1e-10);

    // Test with pipe operator
    auto result2 = s1 | df::utils::make_slice(1, 4);
    EXPECT_EQ(result2.count(), 3);
    EXPECT_EQ(result2.itemSize(), 1);
    EXPECT_NEAR(result2.value(0), 2.0, 1e-10);
    EXPECT_NEAR(result2.value(1), 3.0, 1e-10);
    EXPECT_NEAR(result2.value(2), 4.0, 1e-10);
}

TEST(slice, vector_serie) {
    // Test vector series
    df::GenSerie<double> s2(3, {
                                   1, 2, 3,   // vec1
                                   4, 5, 6,   // vec2
                                   7, 8, 9,   // vec3
                                   10, 11, 12 // vec4
                               });

    auto result = df::utils::slice(1, 3, s2);
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(result.itemSize(), 3);

    auto vec1 = result.array(0);
    EXPECT_NEAR(vec1[0], 4.0, 1e-10);
    EXPECT_NEAR(vec1[1], 5.0, 1e-10);
    EXPECT_NEAR(vec1[2], 6.0, 1e-10);

    auto vec2 = result.array(1);
    EXPECT_NEAR(vec2[0], 7.0, 1e-10);
    EXPECT_NEAR(vec2[1], 8.0, 1e-10);
    EXPECT_NEAR(vec2[2], 9.0, 1e-10);
}

TEST(slice, errors) {
    df::GenSerie<double> s(1, {1, 2, 3, 4});

    // Test start >= count
    EXPECT_THROW(df::utils::slice(4, 5, s), std::out_of_range);

    // Test end > count
    EXPECT_THROW(df::utils::slice(0, 5, s), std::out_of_range);

    // Test start >= end
    EXPECT_THROW(df::utils::slice(2, 2, s), std::invalid_argument);
    EXPECT_THROW(df::utils::slice(3, 2, s), std::invalid_argument);
}

TEST(slice, edge_cases) {
    df::GenSerie<double> s(1, {1, 2, 3, 4});

    // Test slice at start
    auto result1 = df::utils::slice(0, 2, s);
    EXPECT_EQ(result1.count(), 2);
    EXPECT_NEAR(result1.value(0), 1.0, 1e-10);
    EXPECT_NEAR(result1.value(1), 2.0, 1e-10);

    // Test slice at end
    auto result2 = df::utils::slice(2, 4, s);
    EXPECT_EQ(result2.count(), 2);
    EXPECT_NEAR(result2.value(0), 3.0, 1e-10);
    EXPECT_NEAR(result2.value(1), 4.0, 1e-10);
}

RUN_TESTS()