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
#include <dataframe/math/random.h>
#include <dataframe/utils/filter.h>
#include <set>

TEST(Random, basic_int) {
    const size_t n = 1000;
    const int min = -10;
    const int max = 10;

    auto serie = df::random_uniform<int>(n, min, max);

    // Check size
    EXPECT_EQ(serie.size(), n);

    // Check bounds
    bool in_bounds = true;
    serie.forEach([&](int value, size_t) {
        if (value < min || value > max) {
            in_bounds = false;
        }
    });
    EXPECT_TRUE(in_bounds);

    // Check distribution (should have reasonable spread)
    std::set<int> unique_values;
    serie.forEach([&](int value, size_t) { unique_values.insert(value); });

    // With 1000 samples between -10 and 10, we should have most values
    // represented
    EXPECT_GT(unique_values.size(), 15); // At least 75% of possible values
}

TEST(Random, basic_double) {
    const size_t n = 1000;
    const double min = 0.0;
    const double max = 1.0;

    auto serie = df::random_uniform<double>(n, min, max);

    // Check size
    EXPECT_EQ(serie.size(), n);

    // Check bounds
    bool in_bounds = true;
    serie.forEach([&](double value, size_t) {
        if (value < min || value > max) {
            in_bounds = false;
        }
    });
    EXPECT_TRUE(in_bounds);

    // Check that we have good spread of values
    double sum = 0.0;
    serie.forEach([&](double value, size_t) { sum += value; });

    // Mean should be approximately 0.5
    double mean = sum / n;
    EXPECT_NEAR(mean, 0.5, 0.1);
}

TEST(Random, generic_type) {
    // Test with float
    auto float_serie = df::random_uniform<float>(100, -0.0f, 1.0f);
    EXPECT_EQ(float_serie.size(), 100);

    // Test with long
    auto long_serie = df::random_uniform<long>(50, -100L, 100L);
    EXPECT_EQ(long_serie.size(), 50);
}

TEST(Random, empty) {
    auto serie = df::random_uniform<int>(0, 0, 10);
    EXPECT_TRUE(serie.empty());
}

TEST(Random, single_value) {
    auto serie = df::random_uniform<int>(1, 42, 42);
    EXPECT_EQ(serie.size(), 1);
    EXPECT_EQ(serie[0], 42);
}

TEST(Random, pipe_example) {
    using namespace df;

    // Create random series and transform them
    auto result = df::random_uniform<double>(100, 0.0, 1.0) |
                  df::bind_map([](double v, size_t) { return v * 2.0; }) |
                  df::bind_filter([](double v, size_t) { return v > 1.0; });

    EXPECT_GT(result.size(), 0);
    EXPECT_LT(result.size(), 100);
}

// ===============================================

// Uniform Distribution Tests
TEST(Random, uniform_distribution) {
    const size_t n = 10000;
    auto serie = df::random_uniform<double>(n, -1.0, 1.0);

    // Check size and bounds
    EXPECT_EQ(serie.size(), n);

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    double sum = 0.0;

    serie.forEach([&](double value, size_t) {
        min = std::min(min, value);
        max = std::max(max, value);
        sum += value;
    });

    // Check bounds
    EXPECT_GT(min, -1.01); // Allow small floating point deviation
    EXPECT_LT(max, 1.01);

    // Mean should be close to 0 for [-1,1]
    double mean = sum / n;
    EXPECT_NEAR(mean, 0.0, 0.1);
}

// Normal Distribution Tests
TEST(Random, normal_distribution) {
    const size_t n = 10000;
    const double expected_mean = 5.0;
    const double expected_stddev = 2.0;

    auto serie = df::random_normal<double>(n, expected_mean, expected_stddev);
    EXPECT_EQ(serie.size(), n);

    // Calculate actual mean and standard deviation
    double sum = 0.0;
    serie.forEach([&](double value, size_t) { sum += value; });
    double actual_mean = sum / n;

    double sum_sq_diff = 0.0;
    serie.forEach([&](double value, size_t) {
        double diff = value - actual_mean;
        sum_sq_diff += diff * diff;
    });
    double actual_stddev = std::sqrt(sum_sq_diff / (n - 1));

    // Check that mean and stddev are close to expected values
    EXPECT_NEAR(actual_mean, expected_mean, 0.2);
    EXPECT_NEAR(actual_stddev, expected_stddev, 0.2);

    // Count values within standard deviations
    int within_1_stddev = 0;
    int within_2_stddev = 0;
    int within_3_stddev = 0;

    serie.forEach([&](double value, size_t) {
        double z_score = std::abs(value - actual_mean) / actual_stddev;
        if (z_score <= 1.0)
            within_1_stddev++;
        if (z_score <= 2.0)
            within_2_stddev++;
        if (z_score <= 3.0)
            within_3_stddev++;
    });

    // Check empirical rule (68-95-99.7 rule)
    double p1 = static_cast<double>(within_1_stddev) / n;
    double p2 = static_cast<double>(within_2_stddev) / n;
    double p3 = static_cast<double>(within_3_stddev) / n;

    EXPECT_NEAR(p1, 0.68, 0.05);  // ~68% within 1 stddev
    EXPECT_NEAR(p2, 0.95, 0.05);  // ~95% within 2 stddev
    EXPECT_NEAR(p3, 0.997, 0.02); // ~99.7% within 3 stddev
}

// Bernoulli Distribution Tests
TEST(Random, bernoulli_distribution) {
    const size_t n = 10000;
    const double p = 0.7; // probability of success

    auto serie = df::random_bernoulli<int>(n, p);
    EXPECT_EQ(serie.size(), n);

    int successes = 0;
    serie.forEach([&](int value, size_t) {
        EXPECT_TRUE(value == 0 || value == 1);
        if (value == 1)
            successes++;
    });

    double observed_p = static_cast<double>(successes) / n;
    EXPECT_NEAR(observed_p, p, 0.05);
}

// Poisson Distribution Tests
TEST(Random, poisson_distribution) {
    const size_t n = 10000;
    const double lambda = 3.0;

    auto serie = df::random_poisson<int>(n, lambda);
    EXPECT_EQ(serie.size(), n);

    double sum = 0;
    int max_val = 0;
    serie.forEach([&](int value, size_t) {
        EXPECT_GE(value, 0);
        sum += value;
        max_val = std::max(max_val, value);
    });

    // Mean should be close to lambda
    double mean = sum / n;
    EXPECT_NEAR(mean, lambda, 0.2);

    // Variance should be close to lambda for Poisson
    double sum_sq_diff = 0.0;
    serie.forEach([&](int value, size_t) {
        double diff = value - mean;
        sum_sq_diff += diff * diff;
    });
    double variance = sum_sq_diff / (n - 1);
    EXPECT_NEAR(variance, lambda, 0.3);
}

// Random Sampling Tests
TEST(Random, sampling_with_replacement) {
    const size_t n = 1000;
    std::vector<double> population = {1.0, 2.0, 3.0, 4.0, 5.0};

    auto serie = df::random_sample<double>(n, population, true);
    EXPECT_EQ(serie.size(), n);

    // Check that all values come from population
    bool all_valid = true;
    serie.forEach([&](double value, size_t) {
        if (std::find(population.begin(), population.end(), value) ==
            population.end()) {
            all_valid = false;
        }
    });
    EXPECT_TRUE(all_valid);

    // With replacement, we should see all values from population
    std::set<double> unique_values;
    serie.forEach([&](double value, size_t) { unique_values.insert(value); });
    EXPECT_EQ(unique_values.size(), population.size());
}

TEST(Random, sampling_without_replacement) {
    const size_t n = 3;
    std::vector<double> population = {1.0, 2.0, 3.0, 4.0, 5.0};

    auto serie = df::random_sample<double>(n, population, false);
    EXPECT_EQ(serie.size(), n);

    // Check that all values are unique
    std::set<double> unique_values;
    serie.forEach([&](double value, size_t) {
        unique_values.insert(value);
        EXPECT_TRUE(std::find(population.begin(), population.end(), value) !=
                    population.end());
    });
    EXPECT_EQ(unique_values.size(), n);
}

// Edge Cases
TEST(Random, distributions_edge_cases) {
    // Normal distribution with zero standard deviation
    auto normal_zero_std = df::random_normal<double>(100, 5.0, 0.0);
    normal_zero_std.forEach(
        [](double value, size_t) { EXPECT_NEAR(value, 5.0, 1e-10); });

    // Bernoulli with p=0 and p=1
    auto bern_zero = df::random_bernoulli<int>(100, 0.0);
    bern_zero.forEach([](int value, size_t) { EXPECT_EQ(value, 0); });

    auto bern_one = df::random_bernoulli<int>(100, 1.0);
    bern_one.forEach([](int value, size_t) { EXPECT_EQ(value, 1); });

    // Poisson with very small lambda
    auto poisson_small = df::random_poisson<int>(100, 0.1);
    int sum = 0;
    poisson_small.forEach([&](int value, size_t) { sum += value; });
    EXPECT_LT(sum, 50); // Should have many zeros
}

// Performance stress test
TEST(Random, performance_large_samples) {
    const size_t n = 1000000;

    double time_uniform =
        TIMING([&]() { auto serie = df::random_uniform<double>(n, 0.0, 1.0); });
    MSG("Uniform generation time (ms): " << time_uniform);

    double time_normal =
        TIMING([&]() { auto serie = df::random_normal<double>(n, 0.0, 1.0); });
    MSG("Normal generation time (ms): " << time_normal);

    double time_poisson =
        TIMING([&]() { auto serie = df::random_poisson<int>(n, 1.0); });
    MSG("Poisson generation time (ms): " << time_poisson);
}

RUN_TESTS();