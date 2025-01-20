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
#include <dataframe/functional/utils/concat.h>

TEST(concat, test1) {
    // Test scalar series
    df::GenSerie<double> s1(1, {1, 2, 3});
    df::GenSerie<double> s2(1, {4, 5, 6});

    auto result1 = df::utils::concat<double>({s1, s2});
    EXPECT_EQ(result1.count(), 6);
    EXPECT_EQ(result1.itemSize(), 1);
    EXPECT_NEAR(result1.value(0), 1.0, 1e-10);
    EXPECT_NEAR(result1.value(5), 6.0, 1e-10);

    // Test vector series
    df::GenSerie<double> v1(3, {1, 2, 3, 4, 5, 6});
    df::GenSerie<double> v2(3, {7, 8, 9, 10, 11, 12});

    auto result2 = df::utils::concat<double>({v1, v2});
    EXPECT_EQ(result2.count(), 4);
    EXPECT_EQ(result2.itemSize(), 3);
    auto arr1 = result2.array(0);
    EXPECT_NEAR(arr1[0], 1.0, 1e-10);
    EXPECT_NEAR(arr1[1], 2.0, 1e-10);
    EXPECT_NEAR(arr1[2], 3.0, 1e-10);

    auto arr4 = result2.array(3);
    EXPECT_NEAR(arr4[0], 10.0, 1e-10);
    EXPECT_NEAR(arr4[1], 11.0, 1e-10);
    EXPECT_NEAR(arr4[2], 12.0, 1e-10);
}

TEST(concat, errors) {
    // Test with different itemSizes
    df::GenSerie<double> s1(1, {1, 2, 3});
    df::GenSerie<double> s2(2, {4, 5, 6, 7});

    EXPECT_THROW(df::utils::concat<double>({s1, s2}), std::invalid_argument);

    // Test with empty vector
    std::vector<df::GenSerie<double>> empty;
    auto result = df::utils::concat<double>(empty);
    EXPECT_EQ(result.count(), 0);
}

TEST(concat, test_variadic) {
    df::GenSerie<double> s1(1, {1, 2});
    df::GenSerie<double> s2(1, {3, 4});
    df::GenSerie<double> s3(1, {5, 6});

    // Using direct concat
    auto result1 = df::utils::concat<double>(s1, s2, s3);
    EXPECT_EQ(result1.count(), 6);

    // Using make_concat with vector
    std::vector<df::GenSerie<double>> others{s2, s3};
    auto result2 = s1 | df::utils::make_concat(others);
    EXPECT_EQ(result2.count(), 6);

    // Using make_concat with variadic arguments
    auto result3 = s1 | df::utils::make_concat<double>(s2, s3);
    EXPECT_EQ(result3.count(), 6);

    // Verify results are the same
    for (uint32_t i = 0; i < result1.count(); ++i) {
        EXPECT_NEAR(result1.value(i), result2.value(i), 1e-10);
        EXPECT_NEAR(result1.value(i), result3.value(i), 1e-10);
    }
}

RUN_TESTS();