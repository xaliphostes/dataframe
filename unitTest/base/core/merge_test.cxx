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
#include <dataframe/core/merge.h>

using namespace df;

TEST(Merge, Basic_Concatenation) {
    auto s1 = Serie<int>({1, 2, 3});
    auto s2 = Serie<int>({4, 5, 6});

    auto result = merge(s1, s2);

    COMPARE_SERIE_VECTOR(result, {1, 2, 3, 4, 5, 6});
}

TEST(Merge, Multiple_Series_Concatenation) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});
    auto s3 = Serie<int>({5, 6});

    auto result = merge(s1, s2, s3);

    COMPARE_SERIE_VECTOR(result, {1, 2, 3, 4, 5, 6});
}

TEST(Merge, Basic_Interleaving) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});

    auto result = merge(MergeMode::Interleave, s1, s2);

    COMPARE_SERIE_VECTOR(result, {1, 3, 2, 4});
}

TEST(Merge, Multiple_Series_Interleaving) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});
    auto s3 = Serie<int>({5, 6});

    auto result = merge(MergeMode::Interleave, s1, s2, s3);

    // df::print(result);
    // COMPARE_SERIE_VECTOR(result, {1, 3, 5, 2, 4, 6});
}

TEST(Merge, Multiple_Series_Interleaving_2) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});
    auto s3 = Serie<int>({5, 6});
    auto s4 = Serie<int>({7, 8});
    auto s5 = Serie<int>({9, 10});

    auto result = merge(MergeMode::Interleave, s1, s2, s3, s4, s5);

    // df::print(result);
    // COMPARE_SERIE_VECTOR(result, {1, 3, 5, 7, 9, 2, 4, 6, 8, 10});
}

TEST(Merge, Different_Length_Series_Concatenation) {
    auto s1 = Serie<int>({1, 2, 3});
    auto s2 = Serie<int>({4});

    auto result = merge(s1, s2);

    COMPARE_SERIE_VECTOR(result, {1, 2, 3, 4});
}

TEST(Merge, Different_Length_Series_Interleaving) {
    auto s1 = Serie<int>({1, 2, 3});
    auto s2 = Serie<int>({4});

    auto result = merge(MergeMode::Interleave, s1, s2);

    COMPARE_SERIE_VECTOR(result, {1, 4, 2, 3});
}

TEST(Merge, Empty_Series_Concatenation) {
    auto s1 = Serie<int>({});
    auto s2 = Serie<int>({1, 2});

    auto result1 = merge(s1, s2);
    auto result2 = merge(s2, s1);

    COMPARE_SERIE_VECTOR(result1, {1, 2});
    COMPARE_SERIE_VECTOR(result2, {1, 2});
}

TEST(Merge, Empty_Series_Interleaving) {
    auto s1 = Serie<int>({});
    auto s2 = Serie<int>({1, 2});

    auto result1 = merge(MergeMode::Interleave, s1, s2);
    auto result2 = merge(MergeMode::Interleave, s2, s1);

    COMPARE_SERIE_VECTOR(result1, {1, 2});
    COMPARE_SERIE_VECTOR(result2, {1, 2});
}

TEST(Merge, Different_Types) {
    auto s1 = Serie<std::string>({"a", "b"});
    auto s2 = Serie<std::string>({"c", "d"});

    auto result1 = merge(s1, s2);
    auto result2 = merge(MergeMode::Interleave, s1, s2);

    COMPARE_SERIE_VECTOR(result1, {"a", "b", "c", "d"});
    COMPARE_SERIE_VECTOR(result2, {"a", "c", "b", "d"});
}

TEST(Merge, Pipe_Operator) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});
    auto s3 = Serie<int>({5, 6});

    // Test concatenation with pipe
    auto result1 = s1 | bind_merge(s2) | bind_merge(s3);
    df::print(result1);
    // COMPARE_SERIE_VECTOR(result1, {1, 2, 3, 4, 5, 6});

    // Test interleaving with pipe
    auto result2 = s1 | bind_interleave(s2) | bind_interleave(s3);
    df::print(result2);
    // COMPARE_SERIE_VECTOR(result2, {1, 3, 5, 2, 4, 6});
}

TEST(Merge, Helper_Functions) {
    auto s1 = Serie<int>({1, 2});
    auto s2 = Serie<int>({3, 4});
    auto s3 = Serie<int>({5, 6});

    auto result = interleave(s1, s2, s3);
    // COMPARE_SERIE_VECTOR(result, {1, 3, 5, 2, 4, 6});
}

TEST(Merge, Large_Series) {
    std::vector<int> v1(1000);
    std::vector<int> v2(1000);
    for (int i = 0; i < 1000; ++i) {
        v1[i] = i * 2;
        v2[i] = i * 2 + 1;
    }

    auto s1 = Serie<int>(v1);
    auto s2 = Serie<int>(v2);

    auto result = merge(MergeMode::Interleave, s1, s2);
    EXPECT_TRUE(result.size() == 2000);
    for (size_t i = 0; i < 2000; ++i) {
        EXPECT_TRUE(result[i] == static_cast<int>(i));
    }
}

RUN_TESTS();