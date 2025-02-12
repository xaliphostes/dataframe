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
#include <dataframe/utils/ones.h>

TEST(ones, ones_basic) {
    auto s = df::ones<int>(5);
    EXPECT_TRUE(s.size() == 5);
    for (size_t i = 0; i < s.size(); ++i) {
        EXPECT_TRUE(s[i] == 1);
    }
}

TEST(ones, ones_different_types) {
    // Test with different numeric types
    {
        auto s = df::ones<int>(2);
        EXPECT_TRUE(s[0] == 1);
        EXPECT_STREQ(s.type(), "int");
    }

    {
        auto s = df::ones<double>(2);
        EXPECT_TRUE(s[0] == 1.0);
        EXPECT_STREQ(s.type(), "double");
    }

    {
        auto s = df::ones<float>(2);
        EXPECT_TRUE(s[0] == 1.0f);
        EXPECT_STREQ(s.type(), "float");
    }
}

TEST(ones, empty_series) {
    auto s1 = df::ones<int>(0);
    EXPECT_TRUE(s1.empty());
    EXPECT_TRUE(s1.size() == 0);
}

TEST(ones, custom_type) {
    struct CustomType {
        CustomType() : value(0) {}
        explicit CustomType(int v) : value(v) {}
        bool operator==(const CustomType &other) const {
            return value == other.value;
        }
        int value;
    };

    auto s1 = df::ones<CustomType>(3);
    EXPECT_TRUE(s1.size() == 3);
    for (size_t i = 0; i < s1.size(); ++i) {
        EXPECT_TRUE(s1[i] == CustomType(1));
    }
}

TEST(ones, large_size) {
    const size_t large_size = 1000000;

    auto s1 = df::ones<int>(large_size);
    EXPECT_TRUE(s1.size() == large_size);
    EXPECT_TRUE(s1[0] == 1);
    EXPECT_TRUE(s1[large_size - 1] == 1);
}

TEST(ones, comparison_vector) {
    auto s1 = df::ones<int>(3);
    std::vector<int> v1{1, 1, 1};
    COMPARE_SERIE_VECTOR(s1, v1);
}

RUN_TESTS();