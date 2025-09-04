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
#include <dataframe/core/split.h>

TEST(SplitEqual, BasicSplit) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto splits = df::split(3, numbers);

    EXPECT_EQ(splits.size(), 3);

    // First chunk: 4 elements
    std::vector<int> expected1{1, 2, 3, 4};
    EXPECT_ARRAY_EQ(splits[0].asArray(), expected1);

    // Second chunk: 3 elements
    std::vector<int> expected2{5, 6, 7};
    EXPECT_ARRAY_EQ(splits[1].asArray(), expected2);

    // Third chunk: 3 elements
    std::vector<int> expected3{8, 9, 10};
    EXPECT_ARRAY_EQ(splits[2].asArray(), expected3);
}

TEST(SplitEqual, EvenSplit) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6};

    auto splits = df::split(2, numbers);

    EXPECT_EQ(splits.size(), 2);
    EXPECT_EQ(splits[0].size(), 3);
    EXPECT_EQ(splits[1].size(), 3);
}

TEST(SplitEqual, SingleElement) {
    df::Serie<int> numbers{1};

    auto splits = df::split(3, numbers);

    EXPECT_EQ(splits.size(), 1);
    EXPECT_EQ(splits[0].size(), 1);
}

// TEST(SplitEqual, EmptySeries) {
//     df::Serie<int> empty{};

//     auto splits = df::split(3, empty);

//     EXPECT_EQ(splits.size(), 0);
// }

TEST(SplitEqual, MultipleSeries) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6};
    df::Serie<std::string> labels{"a", "b", "c", "d", "e", "f"};

    auto splits = df::split(2, numbers, labels);

    EXPECT_EQ(splits.size(), 2);

    // Check first split
    auto [numbers1, labels1] = splits[0];
    std::vector<int> expected_numbers1{1, 2, 3};
    std::vector<std::string> expected_labels1{"a", "b", "c"};
    EXPECT_ARRAY_EQ(numbers1.asArray(), expected_numbers1);
    EXPECT_ARRAY_EQ(labels1.asArray(), expected_labels1);

    // Check second split
    auto [numbers2, labels2] = splits[1];
    std::vector<int> expected_numbers2{4, 5, 6};
    std::vector<std::string> expected_labels2{"d", "e", "f"};
    EXPECT_ARRAY_EQ(numbers2.asArray(), expected_numbers2);
    EXPECT_ARRAY_EQ(labels2.asArray(), expected_labels2);
}

TEST(SplitEqual, ThreeSeries) {
    df::Serie<int> s1{1, 2, 3, 4};
    df::Serie<double> s2{1.1, 2.2, 3.3, 4.4};
    df::Serie<std::string> s3{"a", "b", "c", "d"};

    auto splits = df::split(2, s1, s2, s3);
    EXPECT_EQ(splits.size(), 2);

    // Check first split
    auto [split1_1, split1_2, split1_3] = splits[0];
    EXPECT_EQ(split1_1.size(), 2);
    EXPECT_EQ(split1_2.size(), 2);
    EXPECT_EQ(split1_3.size(), 2);

    // Check second split
    auto [split2_1, split2_2, split2_3] = splits[1];
    EXPECT_EQ(split2_1.size(), 2);
    EXPECT_EQ(split2_2.size(), 2);
    EXPECT_EQ(split2_3.size(), 2);
}

TEST(SplitEqual, PipeOperator) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6};

    auto splitter = df::bind_split<int>(2);
    auto splits = numbers | splitter;

    EXPECT_EQ(splits.size(), 2);
    EXPECT_EQ(splits[0].size(), 3);
    EXPECT_EQ(splits[1].size(), 3);
}

TEST(SplitEqual, TooManySplits) {
    df::Serie<int> numbers{1, 2, 3};

    auto splits = df::split(5, numbers);

    // Should adjust to actual size
    EXPECT_EQ(splits.size(), 3);
    EXPECT_EQ(splits[0].size(), 1);
    EXPECT_EQ(splits[1].size(), 1);
    EXPECT_EQ(splits[2].size(), 1);
}

TEST(SplitEqual, many_series_same_type) {
    df::Serie<int> s1{1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    df::Serie<int> s2{21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                         31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
    df::Serie<int> s3{41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                         51, 52, 53, 54, 55, 56, 57, 58, 59, 60};

    {
        auto splits = df::split(2, df::merge(s1, s2, s3));
        COMPARE_SERIE_VECTOR(splits[0],
                             {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                              11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                              21, 22, 23, 24, 25, 26, 27, 28, 29, 30});
        COMPARE_SERIE_VECTOR(splits[1],
                             {31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                              41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                              51, 52, 53, 54, 55, 56, 57, 58, 59, 60});
    }

    {
        auto splits = df::split(4, df::merge(s1, s2, s3));
        COMPARE_SERIE_VECTOR(
            splits[0], {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
        COMPARE_SERIE_VECTOR(splits[1], {16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                         26, 27, 28, 29, 30});
        COMPARE_SERIE_VECTOR(splits[2], {31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                                         41, 42, 43, 44, 45});
        COMPARE_SERIE_VECTOR(splits[3], {46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
                                         56, 57, 58, 59, 60});
    }
}

RUN_TESTS();