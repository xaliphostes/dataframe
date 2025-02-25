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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/math/div.h>
#include <limits>

TEST(Divide, same_type_int) {
    df::Serie<int> serie1({6, 8, 9});
    df::Serie<int> serie2({2, 4, 3});

    auto result = df::div(serie1, serie2);

    std::vector<int> expected = {3, 2, 3};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Divide, same_type_double) {
    df::Serie<double> serie1({6.0, 8.0, 9.0});
    df::Serie<double> serie2({2.0, 4.0, 3.0});

    auto result = df::div(serie1, serie2);

    std::vector<double> expected = {3.0, 2.0, 3.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Divide, different_types) {
    df::Serie<int> serie1({6, 8, 9});
    df::Serie<double> serie2({2.0, 4.0, 3.0});

    // Integer divided by double
    auto result1 = df::div(serie1, serie2);
    std::vector<double> expected1 = {3.0, 2.0, 3.0};
    EXPECT_ARRAY_EQ(result1.asArray(), expected1);

    // Double divided by integer
    auto result2 = df::div(serie2, serie1);
    std::vector<double> expected2 = {1.0 / 3.0, 0.5, 1.0 / 3.0};
    EXPECT_ARRAY_EQ(result2.asArray(), expected2);
}

TEST(Divide, pipeline_operator) {
    df::Serie<int> serie1({6, 8, 9});
    df::Serie<double> serie2({2.0, 4.0, 3.0});

    auto result = serie1 | df::bind_div(serie2);

    std::vector<double> expected = {3.0, 2.0, 3.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Divide, division_by_zero) {
    df::Serie<double> serie1({1.0, 2.0, 3.0});
    df::Serie<double> serie2({1.0, 0.0, 2.0});

    auto result = df::div(serie1, serie2);

    EXPECT_TRUE(std::isfinite(result[0]));
    EXPECT_TRUE(std::isinf(result[1]));
    EXPECT_TRUE(std::isfinite(result[2]));
}

TEST(Divide, special_values) {
    df::Serie<double> serie1({std::numeric_limits<double>::infinity(),
                              -std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::quiet_NaN(),
                              std::numeric_limits<double>::min(),
                              std::numeric_limits<double>::max(), 0.0});

    df::Serie<double> serie2(
        {2.0, 2.0, 1.0, 2.0, 2.0, std::numeric_limits<double>::infinity()});

    auto result = df::div(serie1, serie2);
    df::print(result);

    // Check each value individually
    EXPECT_TRUE(std::isinf(result[0])); // inf/2 = inf
    EXPECT_TRUE(std::isinf(result[1])); // -inf/2 = -inf
    EXPECT_TRUE(std::isnan(result[2])); // NaN/1 = NaN
    EXPECT_EQ(result[3], std::numeric_limits<double>::min() / 2.0);

    // EXPECT_TRUE(std::isinf(result[4])); // max/2 might be inf // Need C++23 !!!
    EXPECT_TRUE(result[4] > 1e50);
    
    EXPECT_EQ(result[5], 0.0);          // 0/inf = 0
}

TEST(Divide, empty_series) {
    df::Serie<double> empty1;
    df::Serie<double> empty2;

    auto result = df::div(empty1, empty2);
    EXPECT_TRUE(result.empty());
}

TEST(Divide, size_mismatch) {
    df::Serie<int> serie1({1, 2, 3});
    df::Serie<int> serie2({1, 2});

    EXPECT_THROW(df::div(serie1, serie2), std::runtime_error);
}

TEST(Divide, type_promotion) {
    // int / int -> int
    df::Serie<int> i1({6, 8}), i2({2, 4});
    auto r1 = df::div(i1, i2);
    static_assert(std::is_same<decltype(r1)::value_type, int>::value,
                  "int / int should give int");

    // int / double -> double
    df::Serie<double> d1({2.0, 4.0});
    auto r2 = df::div(i1, d1);
    static_assert(std::is_same<decltype(r2)::value_type, double>::value,
                  "int / double should give double");

    // float / double -> double
    df::Serie<float> f1({6.0f, 8.0f});
    auto r3 = df::div(f1, d1);
    static_assert(std::is_same<decltype(r3)::value_type, double>::value,
                  "float / double should give double");
}

TEST(Divide, fractional_results) {
    df::Serie<int> serie1({5, 7, 10});
    df::Serie<int> serie2({2, 3, 4});

    auto result1 = df::div(serie1, serie2);
    // Integer division should truncate
    std::vector<int> expected1 = {2, 2, 2};
    EXPECT_ARRAY_EQ(result1.asArray(), expected1);

    // Same operation with doubles should give fractional results
    df::Serie<double> serie3({5.0, 7.0, 10.0});
    df::Serie<double> serie4({2.0, 3.0, 4.0});
    auto result2 = df::div(serie3, serie4);
    std::vector<double> expected2 = {2.5, 7.0 / 3.0, 2.5};
    EXPECT_ARRAY_EQ(result2.asArray(), expected2);
}

TEST(Divide, precision_edge_cases) {
    df::Serie<double> serie1({1e-308, // Very small number
                              1e308,  // Very large number
                              1.0});

    df::Serie<double> serie2({1e-308, // Very small number
                              1e308,  // Very large number
                              3.0});

    auto result = df::div(serie1, serie2);

    EXPECT_NEAR(result[0], 1.0, 1e-10);       // Small/Small ≈ 1
    EXPECT_NEAR(result[1], 1.0, 1e-10);       // Large/Large ≈ 1
    EXPECT_NEAR(result[2], 1.0 / 3.0, 1e-10); // Regular division
}

RUN_TESTS();