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

#include "../../TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/math/add.h>
#include <dataframe/stats/moving_avg.h>
#include <limits>

TEST(MovingAvg, Basic) {
    df::Serie<double> series{1.0, 2.0, 3.0, 4.0, 5.0};

    // Test window size 1 (should return the same series)
    auto ma1 = df::stats::moving_avg(series, 1);
    EXPECT_ARRAY_EQ(ma1.data(), series.data());

    // Test window size 2
    auto ma2 = df::stats::moving_avg(series, 2);
    std::vector<double> expected2{1.0, 1.5, 2.5, 3.5, 4.5};
    EXPECT_ARRAY_EQ(ma2.data(), expected2);

    // Test window size 3
    auto ma3 = df::stats::moving_avg(series, 3);
    std::vector<double> expected3{1.0, 1.5, 2.0, 3.0, 4.0};
    EXPECT_ARRAY_EQ(ma3.data(), expected3);

    // Test window size equal to series length
    auto ma5 = df::stats::moving_avg(series, 5);
    std::vector<double> expected5{1.0, 1.5, 2.0, 2.5, 3.0};
    EXPECT_ARRAY_EQ(ma5.data(), expected5);

    // Test window size greater than series length
    auto ma10 = df::stats::moving_avg(series, 10);
    std::vector<double> expected10{1.0, 1.5, 2.0, 2.5, 3.0};
    EXPECT_ARRAY_EQ(ma10.data(), expected10);
}

TEST(MovingAvg, VectorTypes) {
    // Test with Vector3 type
    df::Serie<Vector3> vector_series{
        {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}, {10.0, 11.0, 12.0}};

    auto ma2 = df::stats::moving_avg(vector_series, 2);
    std::vector<Vector3> expected{
        {1.0, 2.0, 3.0}, // First element
        {2.5, 3.5, 4.5}, // Average of first two elements
        {5.5, 6.5, 7.5}, // Average of elements 2-3
        {8.5, 9.5, 10.5} // Average of elements 3-4
    };

    EXPECT_EQ(ma2.size(), expected.size());
    for (size_t i = 0; i < ma2.size(); ++i) {
        EXPECT_ARRAY_NEAR(ma2[i], expected[i], 1e-10);
    }
}

TEST(MovingAvg, EdgeCases) {
    // Test with empty series (should throw)
    df::Serie<double> empty;
    EXPECT_THROW(df::stats::moving_avg(empty, 3), std::runtime_error);

    // Test with zero window size (should throw)
    df::Serie<double> series{1.0, 2.0, 3.0};
    EXPECT_THROW(df::stats::moving_avg(series, 0), std::runtime_error);

    // Test with single element series
    df::Serie<double> single{42.0};
    auto ma_single = df::stats::moving_avg(single, 3);
    EXPECT_EQ(ma_single.size(), 1);
    EXPECT_EQ(ma_single[0], 42.0);
}

TEST(MovingAvg, PipelineUsage) {
    df::Serie<double> series{1.0, 2.0, 3.0, 4.0, 5.0};

    // Test in pipeline with explicit type
    auto result1 = series | df::stats::bind_moving_avg<double>(3);
    std::vector<double> expected{1.0, 1.5, 2.0, 3.0, 4.0};
    EXPECT_ARRAY_EQ(result1.data(), expected);

    // Test in more complex pipeline
    auto result2 =
        series |
        df::bind_map([](double x, size_t) { return x * 2; }) // Double values
        | df::stats::bind_moving_avg<double>(3)                     // Moving average
        | df::bind_map([](double x, size_t) { return x + 1; }); // Add 1

    std::vector<double> expected2{3.0, 4.0, 5.0, 7.0, 9.0};
    EXPECT_ARRAY_EQ(result2.data(), expected2);
}

RUN_TESTS()