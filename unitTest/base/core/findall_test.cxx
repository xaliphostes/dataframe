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
#include <dataframe/core/find.h>
#include <dataframe/core/pipe.h>

TEST(FindAll, BasicFindAll) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Find all even numbers
    auto all_evens =
        df::find_all([](int x, int) { return x % 2 == 0; }, series);
    EXPECT_EQ(all_evens.size(), 5);
    EXPECT_ARRAY_EQ(all_evens.data(), std::vector<int>({2, 4, 6, 8, 10}));

    // Find first 3 even numbers only
    auto first_three_evens =
        df::find_all([](int x, int) { return x % 2 == 0; }, series, 3);
    EXPECT_EQ(first_three_evens.size(), 3);
    EXPECT_ARRAY_EQ(first_three_evens.data(), std::vector<int>({2, 4, 6}));
}

TEST(FindAll, FindAllWithIndex) {
    auto series = df::Serie<int>{10, 20, 30, 40, 50};

    // Find all values with their indices
    auto result =
        df::find_all_with_index([](int x, int) { return x > 25; }, series);
    EXPECT_EQ(result.size(), 3);

    auto first_match = result[0];
    EXPECT_EQ(std::get<0>(first_match), 30);
    EXPECT_EQ(std::get<1>(first_match), 2);

    // Find limited number of matches
    auto limited =
        df::find_all_with_index([](int x, int) { return x > 25; }, series, 2);
    EXPECT_EQ(limited.size(), 2);
    EXPECT_EQ(std::get<0>(limited[0]), 30);
    EXPECT_EQ(std::get<0>(limited[1]), 40);
}

TEST(FindAll, EmptyAndNoMatches) {
    auto empty_series = df::Serie<int>{};
    auto series = df::Serie<int>{1, 2, 3, 4, 5};

    // Find_all on empty series should return empty series
    auto empty_result =
        df::find_all([](int x, int) { return x > 0; }, empty_series);
    EXPECT_EQ(empty_result.size(), 0);

    // Find_all with no matches should return empty series
    auto no_matches = df::find_all([](int x, int) { return x > 10; }, series);
    EXPECT_EQ(no_matches.size(), 0);
}

TEST(FindAll, MultiSeriesFindAll) {
    auto series1 = df::Serie<int>{1, 2, 3, 4, 5};
    auto series2 = df::Serie<int>{10, 20, 30, 40, 50};

    // Find all pairs where product > 50
    auto product_matches = df::find_all(
        [](int x, int y, int) { return x * y > 50; }, series1, series2);
    EXPECT_EQ(product_matches.size(), 3);
    EXPECT_ARRAY_EQ(product_matches.data(), std::vector<int>({3, 4, 5}));

    // Find limited number of matches
    auto limited_matches = df::find_all(
        [](int x, int y, int) { return x * y > 50; }, series1, series2, 2);
    EXPECT_EQ(limited_matches.size(), 2);
    EXPECT_ARRAY_EQ(limited_matches.data(), std::vector<int>({3, 4}));
}

TEST(FindAll, PipeUsage) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Test find_all in pipe
    auto result = series | df::bind_find_all([](int x, int) { return x > 5; });
    EXPECT_EQ(result.size(), 5);
    EXPECT_ARRAY_EQ(result.data(), std::vector<int>({6, 7, 8, 9, 10}));

    // Test find_all with limit in pipe
    auto limited =
        series | df::bind_find_all([](int x, int) { return x > 5; }, 3);
    EXPECT_EQ(limited.size(), 3);
    EXPECT_ARRAY_EQ(limited.data(), std::vector<int>({6, 7, 8}));

    // Test find_all_with_index in pipe
    auto with_indices = series | df::bind_find_all_with_index(
                                     [](int x, int) { return x > 5; }, 2);
    EXPECT_EQ(with_indices.size(), 2);
    EXPECT_EQ(std::get<0>(with_indices[0]), 6);
    EXPECT_EQ(std::get<1>(with_indices[0]), 5);
    EXPECT_EQ(std::get<0>(with_indices[1]), 7);
    EXPECT_EQ(std::get<1>(with_indices[1]), 6);
}

RUN_TESTS()