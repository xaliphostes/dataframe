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
#include <dataframe/functional/utils/partition.h>

TEST(partition, test1) {
    // Test scalar partitioning
    df::GenSerie<double> s1(1, {-2, 1, -3, 4, -5, 6});

    auto [negative, positive] =
        df::utils::partition([](double val, uint32_t) { return val < 0; }, s1);

    EXPECT_EQ(negative.count(), 3);
    EXPECT_EQ(positive.count(), 3);
    EXPECT_EQ(negative.itemSize(), 1);
    EXPECT_EQ(positive.itemSize(), 1);

    EXPECT_NEAR(negative.value(0), -2.0, 1e-10);
    EXPECT_NEAR(negative.value(1), -3.0, 1e-10);
    EXPECT_NEAR(negative.value(2), -5.0, 1e-10);

    EXPECT_NEAR(positive.value(0), 1.0, 1e-10);
    EXPECT_NEAR(positive.value(1), 4.0, 1e-10);
    EXPECT_NEAR(positive.value(2), 6.0, 1e-10);
}

TEST(partition, test2) {
    // Test vector partitioning
    df::GenSerie<double> s2(3, {
                                   -1, 0, 0, // vec1 starts negative
                                   1, 0, 0,  // vec2 starts positive
                                   -2, 0, 0, // vec3 starts negative
                                   2, 0, 0   // vec4 starts positive
                               });

    auto [negative, positive] = df::utils::partition(
        [](const std::vector<double> &vec, uint32_t) { return vec[0] < 0; },
        s2);

    EXPECT_EQ(negative.count(), 2);
    EXPECT_EQ(positive.count(), 2);
    EXPECT_EQ(negative.itemSize(), 3);
    EXPECT_EQ(positive.itemSize(), 3);

    auto neg1 = negative.array(0);
    EXPECT_NEAR(neg1[0], -1.0, 1e-10);
    EXPECT_NEAR(neg1[1], 0.0, 1e-10);
    EXPECT_NEAR(neg1[2], 0.0, 1e-10);

    auto pos1 = positive.array(0);
    EXPECT_NEAR(pos1[0], 1.0, 1e-10);
    EXPECT_NEAR(pos1[1], 0.0, 1e-10);
    EXPECT_NEAR(pos1[2], 0.0, 1e-10);
}

TEST(partition, errors) {
    // Test error when using scalar predicate with vector serie
    df::GenSerie<double> s(3, {1, 2, 3, 4, 5, 6});
    EXPECT_THROW(
        df::utils::partition([](double v, uint32_t) { return v > 0; }, s),
        std::invalid_argument);
}

RUN_TESTS();