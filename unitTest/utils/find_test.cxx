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
#include <dataframe/Serie.h>
#include <dataframe/utils/find.h>
#include <dataframe/pipe.h>
#include <cmath>
#include <limits>

TEST(Find, BasicFind) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Find first even number
    auto first_even = df::find([](int x, int) { return x % 2 == 0; }, series);
    EXPECT_TRUE(first_even.has_value());
    EXPECT_EQ(*first_even, 2);

    // Find first number greater than 7
    auto greater_than_7 = df::find([](int x, int) { return x > 7; }, series);
    EXPECT_TRUE(greater_than_7.has_value());
    EXPECT_EQ(*greater_than_7, 8);

    // Find non-existent value
    auto not_found = df::find([](int x, int) { return x > 20; }, series);
    EXPECT_FALSE(not_found.has_value());
}

TEST(Find, FindWithIndex) {
    auto series = df::Serie<int>{10, 20, 30, 40, 50};

    // Find first value with its index
    auto result =
        df::find_with_index([](int x, int) { return x > 25; }, series);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(std::get<0>(*result), 30);
    EXPECT_EQ(std::get<1>(*result), 2);

    // Find using index in predicate
    auto index_based =
        df::find_with_index([](int, int idx) { return idx > 2; }, series);
    EXPECT_TRUE(index_based.has_value());
    EXPECT_EQ(std::get<0>(*index_based), 40);
    EXPECT_EQ(std::get<1>(*index_based), 3);
}

TEST(Find, EmptySeries) {
    auto empty_series = df::Serie<int>{};

    // Find on empty series should return nullopt
    auto result = df::find([](int x, int) { return x > 0; }, empty_series);
    EXPECT_FALSE(result.has_value());

    // Find with index on empty series should return nullopt
    auto result_with_index =
        df::find_with_index([](int x, int) { return x > 0; }, empty_series);
    EXPECT_FALSE(result_with_index.has_value());
}

TEST(Find, MultiSeries) {
    auto series1 = df::Serie<int>{1, 2, 3, 4, 5};
    auto series2 = df::Serie<int>{10, 20, 30, 40, 50};

    // Find based on sum of corresponding elements
    auto sum_find = df::find([](int x, int y, int) { return x + y > 40; },
                             series1, series2);
    EXPECT_TRUE(sum_find.has_value());
    EXPECT_EQ(*sum_find, 4);

    // Find with index based on product
    auto product_find = df::find_with_index(
        [](int x, int y, int) { return x * y > 100; }, series1, series2);
    EXPECT_TRUE(product_find.has_value());
    EXPECT_EQ(std::get<0>(*product_find), 4);
    EXPECT_EQ(std::get<1>(*product_find), 3);
}

TEST(Find, ComplexTypes) {
    auto vectors = df::Serie<Vector3>{
        {1, 0, 0}, {0, 2, 0}, {0, 0, 3}, {1, 1, 1}, {2, 2, 2}};

    // Find first vector with magnitude > 2
    auto large_vector = df::find(
        [](const Vector3 &v, int) {
            double magnitude =
                std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            return magnitude > 2;
        },
        vectors);
    EXPECT_TRUE(large_vector.has_value());
    EXPECT_ARRAY_EQ(*large_vector, Vector3({0, 0, 3}));
}

TEST(Find, PipeUsage) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Test find in pipe
    auto result = series | df::bind_find([](int x, int) { return x > 5; });
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 6);

    // Test find_with_index in pipe
    auto with_index =
        series | df::bind_find_with_index([](int x, int) { return x > 5; });
    EXPECT_TRUE(with_index.has_value());
    EXPECT_EQ(std::get<0>(*with_index), 6);
    EXPECT_EQ(std::get<1>(*with_index), 5);
}

RUN_TESTS()