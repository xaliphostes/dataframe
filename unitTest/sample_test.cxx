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
#include <dataframe/functional/utils/sample.h>

TEST(sample, scalar_series) {
    // Test scalar series
    df::GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6});

    // Sample without replacement
    auto result1 = df::utils::sample(3, s1);
    EXPECT_EQ(result1.count(), 3);
    EXPECT_EQ(result1.itemSize(), 1);

    // Sample with replacement
    auto result2 = df::utils::sample(8, s1, true);
    EXPECT_EQ(result2.count(), 8);
    EXPECT_EQ(result2.itemSize(), 1);

    // Using pipe operator
    auto result3 = s1 | df::utils::make_sample(3);
    EXPECT_EQ(result3.count(), 3);
}

TEST(sample, vector_series) {
    // Test vector series
    df::GenSerie<double> s2(3, {
                                   1, 2, 3,   // vec1
                                   4, 5, 6,   // vec2
                                   7, 8, 9,   // vec3
                                   10, 11, 12 // vec4
                               });

    auto result = df::utils::sample(2, s2);
    EXPECT_EQ(result.count(), 2);
    EXPECT_EQ(result.itemSize(), 3);

    // Each sampled vector should be complete
    auto vec1 = result.array(0);
    EXPECT_EQ(vec1.size(), 3);
}

TEST(sample, errors) {
    df::GenSerie<double> s(1, {1, 2, 3, 4});

    // Test sample size too large without replacement
    EXPECT_THROW(df::utils::sample(5, s), std::invalid_argument);

    // Test empty series
    df::GenSerie<double> empty(1, 0);
    EXPECT_THROW(df::utils::sample(1, empty), std::invalid_argument);
}

TEST(sample, with_replacement_stats) {
    df::GenSerie<double> s(1, {1, 2, 3, 4, 5});

    // Sample many times with replacement to verify randomness
    constexpr int trials = 1000;
    constexpr int sample_size = 1000;

    std::vector<int> counts(5, 0);
    for (int i = 0; i < trials; ++i) {
        auto sampled = df::utils::sample(sample_size, s, true);
        for (uint32_t j = 0; j < sampled.count(); ++j) {
            counts[static_cast<int>(sampled.value(j)) - 1]++;
        }
    }

    // Each value should appear approximately equally often
    // (within reasonable statistical bounds)
    double expected = trials * sample_size / 5.0;
    double tolerance = expected * 0.1; // 10% tolerance
    for (int count : counts) {
        EXPECT_NEAR(count, expected, tolerance);
    }
}

RUN_TESTS()