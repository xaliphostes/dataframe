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
#include <dataframe/Serie.h>
#include <dataframe/pipe.h>
#include <dataframe/utils/map_if.h>

TEST(MapIf, BasicMapIf) {
    auto series = df::Serie<double>{-2, 1, -3, 4, -5, 6};

    // Double only negative numbers
    auto result =
        df::map_if([](double val, size_t) { return val < 0; },
                   [](double val, size_t) { return val * 2; }, series);

    EXPECT_EQ(result.size(), 6);
    EXPECT_ARRAY_EQ(result.data(), std::vector<double>({-4, 1, -6, 4, -10, 6}));
}

TEST(MapIf, UseIndex) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6};

    // Double values at even indices
    auto result = df::map_if([](int, size_t idx) { return idx % 2 == 0; },
                             [](int val, size_t) { return val * 2; }, series);

    EXPECT_EQ(result.size(), 6);
    EXPECT_ARRAY_EQ(result.data(), std::vector<int>({2, 2, 6, 4, 10, 6}));
}

TEST(MapIf, MultiSeries) {
    auto values = df::Serie<double>{10.0, 20.0, 30.0, 40.0, 50.0};
    auto flags = df::Serie<bool>{true, false, true, false, true};

    // Double values where flag is true
    auto result = df::map_if([](double, bool flag, size_t) { return flag; },
                             [](double val, bool, size_t) { return val * 2; },
                             values, flags);

    EXPECT_EQ(result.size(), 5);
    EXPECT_ARRAY_EQ(result.data(),
                    std::vector<double>({20.0, 20.0, 60.0, 40.0, 100.0}));
}

TEST(MapIf, ThreeSeries) {
    auto values = df::Serie<double>{1.0, 2.0, 3.0, 4.0, 5.0};
    auto thresholds = df::Serie<double>{2.0, 2.0, 2.0, 2.0, 2.0};
    auto flags = df::Serie<bool>{true, false, true, false, true};

    // Double values above threshold where flag is true
    auto result =
        df::map_if([](double val, double thresh, bool flag,
                      size_t) { return val > thresh && flag; },
                   [](double val, double, bool, size_t) { return val * 2; },
                   values, thresholds, flags);

    EXPECT_EQ(result.size(), 5);
    EXPECT_ARRAY_EQ(result.data(),
                    std::vector<double>({1.0, 2.0, 6.0, 4.0, 10.0}));
}

TEST(MapIf, PipeUsage) {
    auto series = df::Serie<double>{-2, 1, -3, 4, -5, 6};

    // Double negative numbers using pipe
    auto result =
        series | df::bind_map_if([](double val, size_t) { return val < 0; },
                                 [](double val, size_t) { return val * 2; });

    EXPECT_EQ(result.size(), 6);
    EXPECT_ARRAY_EQ(result.data(), std::vector<double>({-4, 1, -6, 4, -10, 6}));

    // Test with multiple series
    auto flags = df::Serie<bool>{true, false, true, false, true, false};
    auto multi_result =
        series |
        df::bind_map_if(
            [](double val, bool flag, size_t) { return val < 0 && flag; },
            [](double val, bool, size_t) { return val * 2; }, flags);

    EXPECT_EQ(multi_result.size(), 6);
}

TEST(MapIf, DifferentTypes) {
    auto nums = df::Serie<double>{1.5, 2.7, 3.2, 4.8, 5.1};
    auto ints = df::Serie<int>{1, 2, 3, 4, 5};
    auto flags = df::Serie<bool>{true, false, true, false, true};

    // Double values where int is even and flag is true
    auto result = df::map_if(
        [](double, int i, bool flag, size_t) { return i % 2 == 0 && flag; },
        [](double val, int, bool, size_t) { return val * 2; }, nums, ints,
        flags);

    EXPECT_EQ(result.size(), 5);
}

RUN_TESTS()