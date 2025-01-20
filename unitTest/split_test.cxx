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
#include <dataframe/functional/utils/split.h>

TEST(split, single_serie) {
    // Test scalar series
    df::GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    auto parts1 = df::utils::split(3, s1);

    EXPECT_EQ(parts1.size(), 3);
    EXPECT_EQ(parts1[0].count(), 4); // First part gets extra items
    EXPECT_EQ(parts1[1].count(), 3);
    EXPECT_EQ(parts1[2].count(), 3);

    // Test vector series
    df::GenSerie<double> s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    auto parts2 = df::utils::split(2, s2);

    EXPECT_EQ(parts2.size(), 2);
    EXPECT_EQ(parts2[0].count(), 2);
    EXPECT_EQ(parts2[1].count(), 2);
    EXPECT_EQ(parts2[0].itemSize(), 3);
    EXPECT_EQ(parts2[1].itemSize(), 3);
}

TEST(split, multiple_series) {
    df::GenSerie<double> s1(1, {1, 2, 3, 4});
    df::GenSerie<double> s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});

    auto parts = df::utils::split<double>(2, s1, s2);

    EXPECT_EQ(parts.size(), 2);    // Two input series
    EXPECT_EQ(parts[0].size(), 2); // Each split into 2
    EXPECT_EQ(parts[1].size(), 2);

    // Check first series splits
    EXPECT_EQ(parts[0][0].count(), 2);
    EXPECT_EQ(parts[0][1].count(), 2);
    EXPECT_EQ(parts[0][0].itemSize(), 1);

    // Check second series splits
    EXPECT_EQ(parts[1][0].count(), 2);
    EXPECT_EQ(parts[1][1].count(), 2);
    EXPECT_EQ(parts[1][0].itemSize(), 3);
}

TEST(split, errors) {
    df::GenSerie<double> s1(1, {1, 2, 3, 4});
    df::GenSerie<double> s2(1, {1, 2, 3}); // Different count

    EXPECT_THROW(df::utils::split<double>(0, s1), std::invalid_argument);
    EXPECT_THROW(df::utils::split<double>(2, s1, s2), std::invalid_argument);
}

RUN_TESTS();