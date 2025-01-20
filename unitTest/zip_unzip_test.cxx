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
#include <dataframe/functional/utils/unzip.h>
#include <dataframe/functional/utils/zip.h>

TEST(zip_unzip, scalar_series) {
    // Test zipping scalar series
    df::GenSerie<double> s1(1, {1, 2, 3});
    df::GenSerie<double> s2(1, {4, 5, 6});

    auto zipped = df::utils::zip(s1, s2);
    EXPECT_EQ(zipped.itemSize(), 2);
    EXPECT_EQ(zipped.count(), 3);

    // Test unzipping
    auto unzipped = df::utils::unzip(zipped, {1, 1});
    EXPECT_EQ(unzipped.size(), 2);
    EXPECT_EQ(unzipped[0].itemSize(), 1);
    EXPECT_EQ(unzipped[1].itemSize(), 1);

    // Verify values
    EXPECT_NEAR(unzipped[0].value(0), 1.0, 1e-10);
    EXPECT_NEAR(unzipped[1].value(0), 4.0, 1e-10);
}

TEST(zip_unzip, mixed_series) {
    // Test zipping mixed scalar and vector series
    df::GenSerie<double> s1(1, {1, 2, 3});
    df::GenSerie<double> s2(2, {4, 5, 6, 7, 8, 9});
    df::GenSerie<double> s3(1, {10, 11, 12});

    auto zipped = df::utils::zip(s1, s2, s3);
    EXPECT_EQ(zipped.itemSize(), 4); // 1 + 2 + 1
    EXPECT_EQ(zipped.count(), 3);

    // Test unzipping
    auto unzipped = df::utils::unzip(zipped, {1, 2, 1});
    EXPECT_EQ(unzipped.size(), 3);
    EXPECT_EQ(unzipped[0].itemSize(), 1);
    EXPECT_EQ(unzipped[1].itemSize(), 2);
    EXPECT_EQ(unzipped[2].itemSize(), 1);
}

TEST(zip_unzip, errors) {
    df::GenSerie<double> s1(1, {1, 2, 3});
    df::GenSerie<double> s2(1, {4, 5}); // Different count

    // Test mismatched counts
    EXPECT_THROW(df::utils::zip(s1, s2), std::invalid_argument);

    // Test empty itemSizes
    auto zipped = df::utils::zip(s1, s1);
    EXPECT_THROW(df::utils::unzip(zipped, {}), std::invalid_argument);

    // Test mismatched total itemSize
    EXPECT_THROW(df::utils::unzip(zipped, {1}), std::invalid_argument);
}

RUN_TESTS();