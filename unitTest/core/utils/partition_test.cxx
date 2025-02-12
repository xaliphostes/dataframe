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
#include <dataframe/utils/partition.h>
#include <dataframe/Serie.h>

TEST(Split, BasicSplit) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Split into evens and odds
    auto [evens, odds] =
        df::partition(numbers, [](int value, size_t) { return value % 2 == 0; });

    // Check even numbers
    EXPECT_EQ(evens.size(), 5);
    COMPARE_SERIE_VECTOR(evens, {2, 4, 6, 8, 10});

    // Check odd numbers
    EXPECT_EQ(odds.size(), 5);
    COMPARE_SERIE_VECTOR(odds, {1, 3, 5, 7, 9});
}

TEST(Split, EmptySerie) {
    df::Serie<int> empty_serie{};

    auto [matches, non_matches] =
        df::partition(empty_serie, [](int, size_t) { return true; });

    EXPECT_TRUE(matches.empty());
    EXPECT_TRUE(non_matches.empty());
}

TEST(Split, AllMatch) {
    df::Serie<int> numbers{1, 2, 3, 4, 5};

    auto [matches, non_matches] =
        df::partition(numbers, [](int, size_t) { return true; });

    EXPECT_EQ(matches.size(), 5);
    EXPECT_TRUE(non_matches.empty());
    COMPARE_SERIES(matches, numbers);
}

TEST(Split, NoneMatch) {
    df::Serie<int> numbers{1, 2, 3, 4, 5};

    auto [matches, non_matches] =
        df::partition(numbers, [](int, size_t) { return false; });

    EXPECT_TRUE(matches.empty());
    EXPECT_EQ(non_matches.size(), 5);
    COMPARE_SERIES(non_matches, numbers);
}

TEST(Split, WithIndex) {
    df::Serie<int> numbers{10, 20, 30, 40, 50};

    // Split based on index (even/odd indices)
    auto [even_indices, odd_indices] =
        df::partition(numbers, [](int, size_t index) { return index % 2 == 0; });

    // Check elements at even indices (0, 2, 4)
    EXPECT_EQ(even_indices.size(), 3);
    COMPARE_SERIE_VECTOR(even_indices, {10, 30, 50});

    // Check elements at odd indices (1, 3)
    EXPECT_EQ(odd_indices.size(), 2);
    COMPARE_SERIE_VECTOR(odd_indices, {20, 40});
}

TEST(Split, PipeOperator) {
    df::Serie<int> numbers{1, 2, 3, 4, 5};

    auto partitioner =
        df::bind_partition([](int value, size_t) { return value > 3; });

    auto [greater, lesser] = numbers | partitioner;

    std::vector<int> expected_greater{4, 5};
    std::vector<int> expected_lesser{1, 2, 3};

    COMPARE_SERIE_VECTOR(greater, {4, 5});
    COMPARE_SERIE_VECTOR(lesser, {1, 2, 3});
}

RUN_TESTS();