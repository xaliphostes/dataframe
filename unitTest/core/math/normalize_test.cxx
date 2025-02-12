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
#include <dataframe/map.h>
#include <dataframe/math/normalize.h>
#include <dataframe/math/random.h>
#include <dataframe/pipe.h>

TEST(Normalize, basic_double) {
    // Create a simple series
    df::Serie<double> serie{0.0, 1.0, 2.0, 3.0, 4.0};

    // Default normalization to [0,1]
    auto normalized = df::normalize(serie);

    EXPECT_EQ(normalized.size(), 5);
    EXPECT_NEAR(normalized[0], 0.0, 1e-6);
    EXPECT_NEAR(normalized[4], 1.0, 1e-6);
}

TEST(Normalize, custom_range) {
    df::Serie<double> serie{1.0, 2.0, 3.0, 4.0, 5.0};

    // Normalize to [-1, 1]
    auto normalized = df::normalize(serie, -1.0, 1.0);

    EXPECT_NEAR(normalized[0], -1.0, 1e-6);
    EXPECT_NEAR(normalized[4], 1.0, 1e-6);
}

TEST(Normalize, position_type) {
    using Position = std::array<double, 3>;

    df::Serie<Position> positions{Position{0.0, 1.0, 2.0},
                                  Position{1.0, 2.0, 3.0},
                                  Position{2.0, 3.0, 4.0}};

    // Normalize 3D positions
    auto normalized = df::normalize(positions);

    // Check that each component is normalized
    auto sol = df::Serie<Position>{Position{0.0000, 0.2500, 0.5000},
                                   Position{0.2500, 0.5000, 0.7500},
                                   Position{0.5000, 0.7500, 1.0000}};

    normalized.forEach([&](const Position &p, size_t index) {
        EXPECT_NEAR(p[0], sol[index][0], 1e-6);
        EXPECT_NEAR(p[1], sol[index][1], 1e-6);
        EXPECT_NEAR(p[2], sol[index][2], 1e-6);
    });
}

TEST(Normalize, pipe_operations) {
    using namespace df;

    Serie<double> serie{1.0, 2.0, 3.0, 4.0, 5.0};

    // Use in pipe with config
    auto result = serie | bind_normalize(NormalizeConfig<double>{-1.0, 1.0}) |
                  bind_map([](double v, size_t) { return v * 2.0; });

    EXPECT_NEAR(result[0], -2.0, 1e-6);
    EXPECT_NEAR(result[4], 2.0, 1e-6);
}

TEST(Normalize, random_data) {
    using namespace df;

    // Generate random data and normalize it
    auto random_data = df::random_uniform<double>(1000, -100.0, 100.0);
    auto normalized = df::normalize(random_data);

    // Verify bounds
    double min_val = normalized[0];
    double max_val = normalized[0];

    normalized.forEach([&](double v, size_t) {
        min_val = std::min(min_val, v);
        max_val = std::max(max_val, v);
    });

    EXPECT_NEAR(min_val, 0.0, 1e-6);
    EXPECT_NEAR(max_val, 1.0, 1e-6);
}

TEST(Normalize, edge_cases) {
    using namespace df;

    // Empty series
    Serie<double> empty;
    auto norm_empty = normalize(empty);
    EXPECT_TRUE(norm_empty.empty());

    // Single value
    Serie<double> single{42.0};
    auto norm_single = normalize(single);
    EXPECT_EQ(norm_single.size(), 1);
    EXPECT_NEAR(norm_single[0], 0.0, 1e-6); // Maps to target_min

    // Constant series
    Serie<double> constant{5.0, 5.0, 5.0};
    auto norm_constant = normalize(constant);
    EXPECT_EQ(norm_constant.size(), 3);
    norm_constant.forEach([](double v, size_t) {
        EXPECT_NEAR(v, 0.0, 1e-6); // All map to target_min
    });
}

TEST(Normalize, vector_chain) {
    using Position = std::array<double, 3>;

    // Create a chain of operations on Position type
    auto positions =
        df::Serie<Position>{{0.0, 0.0, 0.0}, {1.0, 1.0, 1.0}, {2.0, 2.0, 2.0}};

    auto result = positions |
                  df::bind_normalize(df::NormalizeConfig<double>{-1.0, 1.0}) |
                  df::bind_map([](const Position &p, size_t) {
                      return Position{p[0] * 2.0, p[1] * 2.0, p[2] * 2.0};
                  });

    // Verify transformation
    EXPECT_NEAR(result[0][0], -2.0, 1e-6);
    EXPECT_NEAR(result[2][0], 2.0, 1e-6);
}

RUN_TESTS();