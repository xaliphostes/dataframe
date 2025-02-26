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
#include <dataframe/core/pipe.h>
#include <dataframe/core/skip.h>
#include <string>

TEST(skip, SingleSerie) {
    // Create a test series
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Test skipping first 5 elements
    auto last_five = df::skip(numbers, 5);

    EXPECT_EQ(last_five.size(), 5);
    EXPECT_ARRAY_EQ(last_five.asArray(), std::vector<int>({6, 7, 8, 9, 10}));

    // Test skipping more elements than the series contains
    auto too_many = df::skip(numbers, 15);

    EXPECT_EQ(too_many.size(), 0);
    EXPECT_TRUE(too_many.empty());

    // Test skipping zero elements
    auto all = df::skip(numbers, 0);

    EXPECT_EQ(all.size(), 10);
    EXPECT_ARRAY_EQ(all.asArray(), numbers.asArray());
}

TEST(skip, MultipleSeries) {
    df::Serie<int> numbers{1, 2, 3, 4, 5};
    df::Serie<std::string> names{"Alice", "Bob", "Charlie", "Dave", "Eve"};

    // Test skipping first 3 elements from all series
    auto [num, nam] = df::skip(3, numbers, names);

    EXPECT_EQ(num.size(), 2);
    EXPECT_EQ(nam.size(), 2);

    EXPECT_ARRAY_EQ(num.asArray(), std::vector<int>({4, 5}));
    EXPECT_STREQ(nam[0].c_str(), "Dave");
    EXPECT_STREQ(nam[1].c_str(), "Eve");

    // Test skipping all elements
    auto [empty_num, empty_nam] = df::skip(5, numbers, names);

    EXPECT_EQ(empty_num.size(), 0);
    EXPECT_EQ(empty_nam.size(), 0);
}

TEST(skip, EdgeCases) {
    // Test with empty series
    df::Serie<int> empty_serie;

    auto skip_empty = df::skip(empty_serie, 5);
    EXPECT_EQ(skip_empty.size(), 0);

    // Test with different sized series (should throw)
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{1, 2, 3, 4};

    EXPECT_THROW(df::skip(2, serie1, serie2), std::runtime_error);
}

// Run the tests
RUN_TESTS()
