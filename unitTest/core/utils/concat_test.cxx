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
#include <dataframe/utils/concat.h>

// Test data setup
df::Serie<double> s1{1.0, 2.0, 3.0};
df::Serie<double> s2{4.0, 5.0, 6.0};
df::Serie<double> s3{7.0, 8.0, 9.0};

// ------------------------------------------------

TEST(concat, basic_concat) {
    auto result = df::concat(s1, s2);
    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
}

TEST(concat, multiple_series_concat) {
    auto result = df::concat(s1, s2, s3);
    EXPECT_TRUE(result.size() == 9);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
    EXPECT_TRUE(result[6] == 7.0);
}

TEST(concat, vector_concat) {
    std::vector<df::Serie<double>> series{s1, s2, s3};
    auto result = df::concat(series);
    EXPECT_TRUE(result.size() == 9);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
    EXPECT_TRUE(result[6] == 7.0);
}

TEST(concat, bind_concat_basic) {
    auto bound = df::bind_concat(s2);
    auto result = bound(s1);
    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
}

TEST(concat, bind_concat_multiple) {
    auto bound = df::bind_concat(s2, s3);
    auto result = bound(s1);
    EXPECT_TRUE(result.size() == 9);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
    EXPECT_TRUE(result[6] == 7.0);
}

// Test empty series
TEST(concat, empty_series) {
    df::Serie<double> empty;
    auto result = df::concat(empty, empty);
    EXPECT_TRUE(result.size() == 0);
}

// Test with pipe operator (if available)
TEST(concat, pipe_operator) {
    auto result = s1 | df::bind_concat(s2, s3);
    EXPECT_TRUE(result.size() == 9);
    EXPECT_TRUE(result[0] == 1.0);
    EXPECT_TRUE(result[3] == 4.0);
    EXPECT_TRUE(result[6] == 7.0);
}

RUN_TESTS();