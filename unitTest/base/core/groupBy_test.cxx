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
#include <dataframe/core/groupBy.h>
#include <dataframe/core/pipe.h>
#include <string>

TEST(Utils, GroupBySingleSerie) {
    // Group numbers by their remainder when divided by 3
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    auto grouped = df::groupBy(numbers, [](int n, size_t) { return n % 3; });

    // Check that we have 3 groups
    EXPECT_EQ(grouped.size(), 3);

    // Check the content of each group
    EXPECT_EQ(grouped[0].size(), 4);
    EXPECT_EQ(grouped[1].size(), 4);
    EXPECT_EQ(grouped[2].size(), 4);

    // Verify group contents
    EXPECT_ARRAY_EQ(grouped[0].asArray(), std::vector<int>({3, 6, 9, 12}));
    EXPECT_ARRAY_EQ(grouped[1].asArray(), std::vector<int>({1, 4, 7, 10}));
    EXPECT_ARRAY_EQ(grouped[2].asArray(), std::vector<int>({2, 5, 8, 11}));

    // Test string keys
    auto grouped_by_parity = df::groupBy(
        numbers, [](int n, size_t) { return n % 2 == 0 ? "even" : "odd"; });

    EXPECT_EQ(grouped_by_parity.size(), 2);
    EXPECT_EQ(grouped_by_parity["even"].size(), 6);
    EXPECT_EQ(grouped_by_parity["odd"].size(), 6);

    // Test using indices in key function
    auto grouped_by_position = df::groupBy(numbers, [](int, size_t idx) {
        return idx < 6 ? "first_half" : "second_half";
    });

    EXPECT_EQ(grouped_by_position.size(), 2);
    EXPECT_EQ(grouped_by_position["first_half"].size(), 6);
    EXPECT_EQ(grouped_by_position["second_half"].size(), 6);
}

TEST(Utils, GroupByMultipleSeries) {
    df::Serie<int> ages{25, 18, 35, 62, 17, 42};
    df::Serie<std::string> names{"Alice", "Bob", "Charlie",
                                 "Dave",  "Eve", "Frank"};
    df::Serie<double> salaries{50000.0, 25000.0, 75000.0,
                               60000.0, 0.0,     90000.0};

    // Group by age category
    auto grouped = df::groupBy(
        ages,
        [](int age, size_t) {
            if (age < 18)
                return "minor";
            if (age < 30)
                return "young_adult";
            if (age < 60)
                return "adult";
            return "senior";
        },
        names, salaries);

    // Check group counts
    EXPECT_EQ(grouped.size(), 4);

    // Check specific group contents
    auto [minor_ages, minor_names, minor_salaries] = grouped["minor"];
    EXPECT_EQ(minor_ages.size(), 1);
    EXPECT_EQ(minor_ages[0], 17);
    EXPECT_STREQ(minor_names[0].c_str(), "Eve");
    EXPECT_EQ(minor_salaries[0], 0.0);

    auto [young_adult_ages, young_adult_names, young_adult_salaries] =
        grouped["young_adult"];
    EXPECT_EQ(young_adult_ages.size(), 2);
    EXPECT_ARRAY_EQ(young_adult_ages.asArray(), std::vector<int>({25, 18}));
    EXPECT_STREQ(young_adult_names[0].c_str(), "Alice");
    EXPECT_STREQ(young_adult_names[1].c_str(), "Bob");

    auto [adult_ages, adult_names, adult_salaries] = grouped["adult"];
    EXPECT_EQ(adult_ages.size(), 2);
    EXPECT_ARRAY_EQ(adult_ages.asArray(), std::vector<int>({35, 42}));

    auto [senior_ages, senior_names, senior_salaries] = grouped["senior"];
    EXPECT_EQ(senior_ages.size(), 1);
    EXPECT_EQ(senior_ages[0], 62);
}

TEST(Utils, GroupByPredicate) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Group by even/odd
    auto grouped =
        df::groupByPredicate(numbers, [](int n) { return n % 2 == 0; });

    EXPECT_EQ(grouped.size(), 2);
    EXPECT_EQ(grouped[true].size(), 5);
    EXPECT_EQ(grouped[false].size(), 5);

    EXPECT_ARRAY_EQ(grouped[true].asArray(), std::vector<int>({2, 4, 6, 8, 10}));
    EXPECT_ARRAY_EQ(grouped[false].asArray(), std::vector<int>({1, 3, 5, 7, 9}));
}

TEST(Utils, GroupByWithPipe) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Test bind_groupBy
    auto grouped1 =
        numbers | df::bind_groupBy([](int n, size_t) { return n % 3; });

    EXPECT_EQ(grouped1.size(), 3);
    EXPECT_ARRAY_EQ(grouped1[0].asArray(), std::vector<int>({3, 6, 9}));

    // Test bind_groupByPredicate
    auto grouped2 =
        numbers | df::bind_groupByPredicate([](int n) { return n > 5; });

    EXPECT_EQ(grouped2.size(), 2);
    EXPECT_ARRAY_EQ(grouped2[true].asArray(), std::vector<int>({6, 7, 8, 9, 10}));
    EXPECT_ARRAY_EQ(grouped2[false].asArray(), std::vector<int>({1, 2, 3, 4, 5}));
}

TEST(Utils, GroupByEdgeCases) {
    // Test with empty series
    df::Serie<int> empty_serie;
    auto empty_grouped =
        df::groupBy(empty_serie, [](int n, size_t) { return n % 2; });

    EXPECT_EQ(empty_grouped.size(), 0);

    // Test when all elements map to the same key
    df::Serie<int> same_parity{2, 4, 6, 8, 10};
    auto same_grouped =
        df::groupBy(same_parity, [](int n, size_t) { return n % 2; });

    EXPECT_EQ(same_grouped.size(), 1);
    EXPECT_EQ(same_grouped[0].size(), 5);

    // Test with different sized series (should throw)
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{1, 2, 3, 4};

    EXPECT_THROW(df::groupBy(
                     serie1, [](int n, size_t) { return n % 2; }, serie2),
                 std::runtime_error);
}

// Run the tests
RUN_TESTS()
