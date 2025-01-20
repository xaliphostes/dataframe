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
#include <dataframe/functional/utils/map_if.h>

TEST(map_if, scalar_test) {
    // Test scalar series
    df::GenSerie<double> s1(1, {-2, 1, -3, 4, -5, 6});

    // Double only negative numbers
    auto result = df::utils::map_if(
        [](double val, uint32_t) { return val < 0; }, // predicate
        [](double val, uint32_t) { return val * 2; }, // transform
        s1);

    EXPECT_EQ(result.count(), 6);
    EXPECT_EQ(result.itemSize(), 1);

    // Check transformed negative values
    EXPECT_NEAR(result.value(0), -4.0, 1e-10);  // -2 -> -4
    EXPECT_NEAR(result.value(2), -6.0, 1e-10);  // -3 -> -6
    EXPECT_NEAR(result.value(4), -10.0, 1e-10); // -5 -> -10

    // Check untouched positive values
    EXPECT_NEAR(result.value(1), 1.0, 1e-10);
    EXPECT_NEAR(result.value(3), 4.0, 1e-10);
    EXPECT_NEAR(result.value(5), 6.0, 1e-10);
}

TEST(map_if, vector_test) {
    // Test vector series
    df::GenSerie<double> s2(3, {
                                   -1, 0, 0, // vec1 starts negative
                                   1, 0, 0,  // vec2 starts positive
                                   -2, 0, 0, // vec3 starts negative
                                   2, 0, 0   // vec4 starts positive
                               });

    // Double vectors that start with negative numbers
    auto result = df::utils::map_if(
        [](const std::vector<double> &vec, uint32_t) { return vec[0] < 0; },
        [](const std::vector<double> &vec, uint32_t) {
            std::vector<double> r = vec;
            for (auto &v : r)
                v *= 2;
            return r;
        },
        s2);

    EXPECT_EQ(result.count(), 4);
    EXPECT_EQ(result.itemSize(), 3);

    // Check transformed vectors (started negative)
    auto vec1 = result.array(0);
    EXPECT_NEAR(vec1[0], -2.0, 1e-10); // -1 -> -2
    EXPECT_NEAR(vec1[1], 0.0, 1e-10);  // 0 -> 0
    EXPECT_NEAR(vec1[2], 0.0, 1e-10);  // 0 -> 0

    auto vec3 = result.array(2);
    EXPECT_NEAR(vec3[0], -4.0, 1e-10); // -2 -> -4
    EXPECT_NEAR(vec3[1], 0.0, 1e-10);  // 0 -> 0
    EXPECT_NEAR(vec3[2], 0.0, 1e-10);  // 0 -> 0

    // Check untouched vectors (started positive)
    auto vec2 = result.array(1);
    EXPECT_NEAR(vec2[0], 1.0, 1e-10);
    EXPECT_NEAR(vec2[1], 0.0, 1e-10);
    EXPECT_NEAR(vec2[2], 0.0, 1e-10);
}

RUN_TESTS()