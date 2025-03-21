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
#include <dataframe/core/pipe.h>
#include <dataframe/stats/stats.h>
#include <cmath>
#include <limits>

using namespace df;

TEST(avg, scalar) {
    MSG("Testing avg with scalar types");

    // Integer data
    Serie<int> int_data{1, 2, 3, 4, 5};
    EXPECT_EQ(df::stats::avg(int_data), 3);

    // Double data
    Serie<double> double_data{1.5, 2.5, 3.5, 4.5, 5.5};
    EXPECT_NEAR(df::stats::avg(double_data), 3.5, 1e-10);

    // Negative numbers
    Serie<double> neg_data{-1.0, 0.0, 1.0};
    EXPECT_NEAR(df::stats::avg(neg_data), 0.0, 1e-10);

    // Single value
    Serie<double> single{42.0};
    EXPECT_NEAR(df::stats::avg(single), 42.0, 1e-10);
}

TEST(avg, vectors) {
    MSG("Testing avg with vector types");

    // 2D vectors
    Serie<Vector2> vec2d{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    auto avg2d = df::stats::avg(vec2d);
    EXPECT_ARRAY_NEAR(avg2d, std::vector<double>({3.0, 4.0}), 1e-10);

    // 3D vectors
    Serie<Vector3> vec3d{{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
    auto avg3d = df::stats::avg(vec3d);
    EXPECT_ARRAY_NEAR(avg3d, std::vector<double>({2.5, 3.5, 4.5}), 1e-10);
}

TEST(avg, matrices) {
    MSG("Testing avg with matrix types");

    // 2x2 symmetric matrices
    Serie<std::array<double, 3>> sym2x2{
        {1.0, 0.0, 1.0}, // [1 0; 0 1]
        {2.0, 1.0, 2.0}  // [2 1; 1 2]
    };
    auto avg_sym2x2 = df::stats::avg(sym2x2);
    EXPECT_ARRAY_NEAR(avg_sym2x2, std::vector<double>({1.5, 0.5, 1.5}), 1e-10);

    // 3x3 symmetric matrices
    Serie<std::array<double, 6>> sym3x3{
        {1.0, 0.0, 0.0, 1.0, 0.0, 1.0}, // Identity
        {2.0, 1.0, 1.0, 2.0, 1.0, 2.0}  // General symmetric
    };
    auto avg_sym3x3 = df::stats::avg(sym3x3);
    EXPECT_ARRAY_NEAR(avg_sym3x3,
                      std::vector<double>({1.5, 0.5, 0.5, 1.5, 0.5, 1.5}),
                      1e-10);
}

TEST(avg, edge_cases) {
    MSG("Testing avg edge cases");

    // Empty serie
    Serie<double> empty{};
    EXPECT_THROW(df::stats::avg(empty), std::runtime_error);

    // Very large numbers
    Serie<double> large{1e15, 2e15, 3e15};
    EXPECT_NEAR(df::stats::avg(large), 2e15, 1e5);

    // Very small numbers
    Serie<double> small{1e-15, 2e-15, 3e-15};
    EXPECT_NEAR(df::stats::avg(small), 2e-15, 1e-20);

    // Mixed positive and negative
    Serie<double> mixed{-1e10, 0.0, 1e10};
    EXPECT_NEAR(df::stats::avg(mixed), 0.0, 1e-10);
}

TEST(avg, pipeline) {
    MSG("Testing avg pipeline operations");

    Serie<double> data{1.0, 2.0, 3.0, 4.0, 5.0};

    // Test bind_avg
    auto result = data | df::stats::bind_avg<double>();
    EXPECT_NEAR(result, 3.0, 1e-10);

    // Test bind_avg (alias)
    auto avg_result = data | df::stats::bind_avg<double>();
    EXPECT_NEAR(avg_result, 3.0, 1e-10);

    // Test with vectors in pipeline
    Serie<Vector2> vec2d{{1.0, 2.0}, {3.0, 4.0}};
    auto vec_result = vec2d | df::stats::bind_avg<Vector2>();
    EXPECT_ARRAY_NEAR(vec_result, std::vector<double>({2.0, 3.0}), 1e-10);
}

RUN_TESTS()