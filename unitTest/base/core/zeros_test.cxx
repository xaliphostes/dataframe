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
#include <dataframe/core/zeros.h>

TEST(zeros, zeros_basic) {
    auto s = df::zeros<double>(3);
    EXPECT_TRUE(s.size() == 3);
    for (size_t i = 0; i < s.size(); ++i) {
        EXPECT_TRUE(s[i] == 0.0);
    }
}

TEST(zeros, zeros_different_types) {
    // Test with different numeric types
    {
        auto s = df::zeros<int>(2);
        EXPECT_TRUE(s[0] == 0);
        EXPECT_STREQ(s.type(), "int");
    }

    {
        auto s = df::zeros<double>(2);
        EXPECT_TRUE(s[0] == 0.0);
        EXPECT_STREQ(s.type(), "double");
    }

    {
        auto s = df::zeros<float>(2);
        EXPECT_TRUE(s[0] == 0.0f);
        EXPECT_STREQ(s.type(), "float");
    }
}

TEST(zeros, empty_series) {
    auto s2 = df::zeros<double>(0);
    EXPECT_TRUE(s2.empty());
    EXPECT_TRUE(s2.size() == 0);
}

TEST(zeros, custom_type) {
    struct CustomType {
        CustomType() : value(0) {}
        explicit CustomType(int v) : value(v) {}
        bool operator==(const CustomType &other) const {
            return value == other.value;
        }
        int value;
    };

    auto s2 = df::zeros<CustomType>(3);
    EXPECT_TRUE(s2.size() == 3);
    for (size_t i = 0; i < s2.size(); ++i) {
        EXPECT_TRUE(s2[i] == CustomType(0));
    }
}

TEST(zeros, large_size) {
    const size_t large_size = 1000000;

    auto s2 = df::zeros<double>(large_size);
    EXPECT_TRUE(s2.size() == large_size);
    EXPECT_TRUE(s2[0] == 0.0);
    EXPECT_TRUE(s2[large_size - 1] == 0.0);
}

TEST(zeros, comparison_vector) {
    auto s2 = df::zeros<double>(3);
    std::vector<double> v2{0.0, 0.0, 0.0};
    COMPARE_SERIE_VECTOR(s2, v2);
}

RUN_TESTS();