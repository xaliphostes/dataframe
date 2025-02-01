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

#include "TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/utils/filter.h>
#include <dataframe/pipe.h>
#include <dataframe/map.h>
#include <cmath>

TEST(Filter, BasicFiltering) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Filter even numbers
    auto evens = df::filter([](int x, int) { return x % 2 == 0; }, series);
    EXPECT_EQ(evens.size(), 5);
    EXPECT_ARRAY_EQ(evens.data(), std::vector<int>({2, 4, 6, 8, 10}));

    // -----------------------------------------
    
    // Filter odd numbers
    auto odds = df::filter([](int x, int) { return x % 2 != 0; }, series);
    EXPECT_EQ(odds.size(), 5);
    EXPECT_ARRAY_EQ(odds.data(), std::vector<int>({1, 3, 5, 7, 9}));

    // -----------------------------------------
    
    // Filter numbers greater than 5
    auto greater_than_5 = df::filter([](int x, int) { return x > 5; }, series);
    EXPECT_EQ(greater_than_5.size(), 5);
    EXPECT_ARRAY_EQ(greater_than_5.data(), std::vector<int>({6, 7, 8, 9, 10}));
}

TEST(Filter, EmptySeries) {
    auto empty_series = df::Serie<int>{};
    
    // Filter on empty series should return empty series
    auto filtered = df::filter([](int x, int) { return x > 0; }, empty_series);
    EXPECT_EQ(filtered.size(), 0);
    EXPECT_TRUE(filtered.empty());
}

TEST(Filter, FilterAllOrNone) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5};
    
    // Filter that matches no elements
    auto none = df::filter([](int x, int) { return x > 10; }, series);
    EXPECT_EQ(none.size(), 0);
    EXPECT_TRUE(none.empty());

    // -----------------------------------------
    
    // Filter that matches all elements
    auto all = df::filter([](int x, int) { return x > 0; }, series);
    EXPECT_EQ(all.size(), 5);
    EXPECT_ARRAY_EQ(all.data(), std::vector<int>({1, 2, 3, 4, 5}));
}

TEST(Filter, UseIndex) {
    auto series = df::Serie<int>{10, 20, 30, 40, 50};
    
    // Filter using index (even indices)
    auto even_indices = df::filter([](int, int idx) { return idx % 2 == 0; }, series);
    EXPECT_EQ(even_indices.size(), 3);
    EXPECT_ARRAY_EQ(even_indices.data(), std::vector<int>({10, 30, 50}));

    // -----------------------------------------
    
    // Filter using both value and index
    auto value_and_index = df::filter(
        [](int x, int idx) { return x > 20 && idx % 2 == 0; }, 
        series
    );
    EXPECT_EQ(value_and_index.size(), 2);
    EXPECT_ARRAY_EQ(value_and_index.data(), std::vector<int>({30, 50}));
}

TEST(Filter, MultiSeries) {
    auto series1 = df::Serie<int>{1, 2, 3, 4, 5};
    auto series2 = df::Serie<int>{10, 20, 30, 40, 50};
    
    // Filter based on sum of corresponding elements
    auto sum_filter = df::filter(
        [](int x, int y, int) { return x + y > 40; },
        series1,
        series2
    );
    EXPECT_EQ(sum_filter.size(), 2);
    EXPECT_ARRAY_EQ(sum_filter.data(), std::vector<int>({4, 5}));

    // -----------------------------------------
    
    // Filter based on product of corresponding elements
    auto product_filter = df::filter(
        [](int x, int y, int) { return x * y > 100; },
        series1,
        series2
    );
    EXPECT_EQ(product_filter.size(), 2);
    EXPECT_ARRAY_EQ(product_filter.data(), std::vector<int>({4, 5}));
}

TEST(Filter, ComplexTypes) {
    auto vectors = df::Serie<Vector3>{
        {1, 0, 0},
        {0, 2, 0},
        {0, 0, 3},
        {1, 1, 1},
        {2, 2, 2}
    };
    
    // Filter vectors with magnitude > 2
    auto large_vectors = df::filter([](const Vector3& v, int) {
        double magnitude = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        return magnitude > 2;
    }, vectors);
    EXPECT_EQ(large_vectors.size(), 2);
    EXPECT_ARRAY_EQ(large_vectors[0], Vector3({0, 0, 3}));
    EXPECT_ARRAY_EQ(large_vectors[1], Vector3({2, 2, 2}));

    // -----------------------------------------
    
    // Filter vectors with any component > 1
    auto large_component = df::filter([](const Vector3& v, int) {
        return v[0] > 1 || v[1] > 1 || v[2] > 1;
    }, vectors);
    EXPECT_EQ(large_component.size(), 3);
    EXPECT_ARRAY_EQ(large_component[0], Vector3({0, 2, 0}));
    EXPECT_ARRAY_EQ(large_component[1], Vector3({0, 0, 3}));
    EXPECT_ARRAY_EQ(large_component[2], Vector3({2, 2, 2}));
}

TEST(Filter, PipeUsage) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Test filter in pipe
    auto result = series | df::bind_filter([](int x, int) { return x % 2 == 0; });
    EXPECT_EQ(result.size(), 5);
    EXPECT_ARRAY_EQ(result.data(), std::vector<int>({2, 4, 6, 8, 10}));

    // -----------------------------------------
    
    // Test multiple filters in pipe
    auto multi_filter = series 
        | df::bind_filter([](int x, int) { return x % 2 == 0; })
        | df::bind_filter([](int x, int) { return x > 5; });
    EXPECT_EQ(multi_filter.size(), 3);
    EXPECT_ARRAY_EQ(multi_filter.data(), std::vector<int>({6, 8, 10}));
}

RUN_TESTS()