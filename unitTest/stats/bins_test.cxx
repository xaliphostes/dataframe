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
 */

#include "../TEST.h"
#include <dataframe/pipe.h>
#include <dataframe/stats/bins.h>

using namespace df;

TEST(bins, auto_range) {
    MSG("Testing bins with automatic range detection");

    // Test uniform distribution
    Serie<double> uniform{0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    auto result = bins(uniform, 5);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<size_t>({2, 2, 2, 2, 2}));

    // Test clustered data
    Serie<double> clustered{1.0, 1.1, 1.2, 5.0, 5.1, 9.0, 9.1, 9.2};
    auto clustered_result = bins(clustered, 5);
    EXPECT_ARRAY_EQ(clustered_result.asArray(),
                    std::vector<size_t>({3, 0, 2, 0, 3}));

    // Test pipeline
    auto pipe_result = uniform | bind_bins<double>(5);
    EXPECT_ARRAY_EQ(pipe_result.asArray(),
                    std::vector<size_t>({2, 2, 2, 2, 2}));
}

TEST(bins, fixed_range) {
    MSG("Testing bins with fixed range");

    // Test data with values outside range
    Serie<double> data{-1.0, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 10.0};
    auto result = bins(data, 5, 0.0, 5.0);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<size_t>({2, 1, 1, 1, 3}));

    // Test pipeline
    auto pipe_result = data | bind_bins<double>(5, 0.0, 5.0);
    EXPECT_ARRAY_EQ(pipe_result.asArray(),
                    std::vector<size_t>({2, 1, 1, 1, 3}));
}

TEST(bins, edge_cases) {
    MSG("Testing bins edge cases");

    // Empty serie
    Serie<double> empty{};
    EXPECT_THROW(bins(empty, 5), std::runtime_error);

    // Single value
    Serie<double> single{1.0};
    auto single_result = bins(single, 5);
    EXPECT_EQ(single_result.size(), 5);
    EXPECT_EQ(single_result.asArray()[0], 1);

    // Invalid parameters
    Serie<double> data{1.0, 2.0, 3.0};
    EXPECT_THROW(bins(data, 0), std::invalid_argument); // Zero bins
    EXPECT_THROW(bins(data, 5, 1.0, 1.0),
                 std::invalid_argument); // Invalid range
    EXPECT_THROW(bins(data, 5, 2.0, 1.0),
                 std::invalid_argument); // Reversed range
}

TEST(bins, integer_data) {
    MSG("Testing bins with integer data");

    // Test integer data with auto range
    Serie<int> data{1, 2, 2, 3, 3, 3, 4, 4, 5};
    auto result = bins(data, 3);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<size_t>({1, 2, 4}));

    // Test integer data with fixed range
    auto range_result = bins(data, 3, 0, 6);
    EXPECT_ARRAY_EQ(range_result.asArray(), std::vector<size_t>({1, 5, 3}));
}

RUN_TESTS()