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
#include <dataframe/core/pipe.h>
#include <dataframe/stats/stats.h>
#include <limits>
#include <vector>

TEST(Stats, Mean) {
    // Test with integer values
    df::Serie<int> ints{2, 4, 4, 4, 5, 5, 7, 9};
    EXPECT_EQ(df::stats::mean(ints), 5);

    // Test with floating point values
    df::Serie<double> doubles{2.5, 4.5, 6.5, 8.5};
    EXPECT_NEAR(df::stats::mean(doubles), 5.5, 1e-10);

    // Test with vector values
    df::Serie<Vector2> vectors{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    Vector2 mean_vec = df::stats::mean(vectors);
    EXPECT_NEAR(mean_vec[0], 3.0, 1e-10);
    EXPECT_NEAR(mean_vec[1], 4.0, 1e-10);

    // Test empty series
    df::Serie<int> empty;
    EXPECT_THROW(df::stats::mean(empty), std::runtime_error);
}

TEST(Stats, Variance) {
    // Test population variance
    df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double pop_var = df::stats::variance(values, true);
    EXPECT_NEAR(pop_var, 4.0, 1e-10);

    // Test sample variance
    double sample_var = df::stats::variance(values);
    EXPECT_NEAR(sample_var, 4.571428571428571, 1e-10);

    // Test with single value (variance should be 0)
    df::Serie<double> single{42.0};
    EXPECT_EQ(df::stats::variance(single), 0.0);

    // Test with vector values
    df::Serie<Vector2> vectors{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    auto vec_var = df::stats::variance(vectors);
    EXPECT_NEAR(vec_var[0], 4.0, 1e-10);
    EXPECT_NEAR(vec_var[1], 4.0, 1e-10);

    // Test empty series
    df::Serie<double> empty;
    EXPECT_THROW(df::stats::variance(empty), std::runtime_error);
}

TEST(Stats, StandardDeviation) {
    // Test population standard deviation
    df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    double pop_std = df::stats::std_dev(values, true);
    EXPECT_NEAR(pop_std, 2.0, 1e-10);

    // Test sample standard deviation
    double sample_std = df::stats::std_dev(values);
    EXPECT_NEAR(sample_std, 2.138089935299395, 1e-10);

    // Test with single value (std dev should be 0)
    df::Serie<double> single{42.0};
    EXPECT_EQ(df::stats::std_dev(single), 0.0);

    // Test empty series
    df::Serie<double> empty;
    EXPECT_THROW(df::stats::std_dev(empty), std::runtime_error);
}

TEST(Stats, Median) {
    // Test with odd number of elements
    df::Serie<int> odd{5, 1, 8, 3, 7};
    EXPECT_EQ(df::stats::median(odd), 5.0);

    // Test with even number of elements
    df::Serie<int> even{5, 1, 8, 3, 7, 9};
    EXPECT_EQ(df::stats::median(even), 6.0);

    // Test with single value
    df::Serie<int> single{42};
    EXPECT_EQ(df::stats::median(single), 42.0);

    // Test with vector values
    df::Serie<Vector2> vectors{{1.0, 6.0}, {3.0, 4.0}, {5.0, 2.0}};
    auto med = df::stats::median(vectors);
    EXPECT_NEAR(med[0], 3.0, 1e-10);
    EXPECT_NEAR(med[1], 4.0, 1e-10);

    // Test empty series
    df::Serie<int> empty;
    EXPECT_THROW(df::stats::median(empty), std::runtime_error);
}

TEST(Stats, Quantile) {
    // Test various quantiles
    df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    // Minimum (0th quantile)
    EXPECT_NEAR(df::stats::quantile(values, 0.0), 1.0, 1e-10);

    // First quartile (25th percentile)
    EXPECT_NEAR(df::stats::quantile(values, 0.25), 3.0, 1e-10);

    // Median (50th percentile)
    EXPECT_NEAR(df::stats::quantile(values, 0.5), 5.0, 1e-10);

    // Third quartile (75th percentile)
    EXPECT_NEAR(df::stats::quantile(values, 0.75), 7.0, 1e-10);

    // Maximum (100th percentile)
    EXPECT_NEAR(df::stats::quantile(values, 1.0), 9.0, 1e-10);

    // Test interpolation (20th percentile)
    EXPECT_NEAR(df::stats::quantile(values, 0.2), 2.6, 1e-10);

    // Test invalid quantile
    EXPECT_THROW(df::stats::quantile(values, -0.1), std::runtime_error);
    EXPECT_THROW(df::stats::quantile(values, 1.1), std::runtime_error);

    // Test empty series
    df::Serie<double> empty;
    EXPECT_THROW(df::stats::quantile(empty, 0.5), std::runtime_error);
}

TEST(Stats, Mode) {
    // Test with single mode
    df::Serie<int> values{1, 2, 2, 3, 3, 3, 4, 4, 5};
    EXPECT_EQ(df::stats::mode(values), 3);

    // Test with strings
    df::Serie<std::string> words{"apple", "banana", "apple", "orange", "apple"};
    EXPECT_EQ(df::stats::mode(words), "apple");

    // Test with single value
    df::Serie<int> single{42};
    EXPECT_EQ(df::stats::mode(single), 42);

    // Test empty series
    df::Serie<int> empty;
    EXPECT_THROW(df::stats::mode(empty), std::runtime_error);
}

TEST(Stats, Summary) {
    df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
    auto stats = df::stats::summary(values);

    EXPECT_EQ(stats["count"], 9.0);
    EXPECT_NEAR(stats["min"], 1.0, 1e-10);
    EXPECT_NEAR(stats["q1"], 3.0, 1e-10);
    EXPECT_NEAR(stats["median"], 5.0, 1e-10);
    EXPECT_NEAR(stats["q3"], 7.0, 1e-10);
    EXPECT_NEAR(stats["max"], 9.0, 1e-10);
    EXPECT_NEAR(stats["mean"], 5.0, 1e-10);
    EXPECT_NEAR(stats["std_dev"], 2.7386127875258306, 1e-10);
}

TEST(Stats, ZScore) {
    df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    auto z = df::stats::z_score(values);

    // Check mean and std of z-scores (should be ~0 and ~1)
    double z_mean = df::stats::mean(z);
    double z_std = df::stats::std_dev(z);

    EXPECT_NEAR(z_mean, 0.0, 1e-10);
    EXPECT_NEAR(z_std, 1.0, 1e-10);

    // Check specific values
    EXPECT_NEAR(z[0], (2.0 - 5.0) / 2.138089935299395, 1e-10);
    EXPECT_NEAR(z[7], (9.0 - 5.0) / 2.138089935299395, 1e-10);

    // Test with zero std dev
    df::Serie<double> same{5.0, 5.0, 5.0};
    EXPECT_THROW(df::stats::z_score(same), std::runtime_error);
}

TEST(Stats, Covariance) {
    // Positive covariance
    df::Serie<double> x{1.0, 2.0, 3.0, 4.0, 5.0};
    df::Serie<double> y{1.0, 2.0, 3.0, 4.0, 5.0};
    EXPECT_NEAR(df::stats::covariance(x, y, false), 2.5, 1e-10);

    // Negative covariance
    df::Serie<double> z{5.0, 4.0, 3.0, 2.0, 1.0};
    EXPECT_NEAR(df::stats::covariance(x, z, false), -2.5, 1e-10);

    // Zero covariance (independent variables)
    df::Serie<double> w{3.0, 3.0, 3.0, 3.0, 3.0};
    EXPECT_NEAR(df::stats::covariance(x, w), 0.0, 1e-6);

    // Different sized series
    df::Serie<double> diff_size{1.0, 2.0};
    EXPECT_THROW(df::stats::covariance(x, diff_size), std::runtime_error);
}

TEST(Stats, Correlation) {
    // Perfect positive correlation
    df::Serie<double> x{1.0, 2.0, 3.0, 4.0, 5.0};
    df::Serie<double> y{2.0, 4.0, 6.0, 8.0, 10.0};
    MSG(df::stats::correlation(x, y));
    EXPECT_NEAR(df::stats::correlation(x, y), 1.0, 1e-10);
    MSG("corelation 1 ok");

    // Perfect negative correlation
    df::Serie<double> z{5.0, 4.0, 3.0, 2.0, 1.0};
    EXPECT_NEAR(df::stats::correlation(x, z), -1.0, 1e-10);
    MSG("corelation -1 ok");

    // Zero correlation
    df::Serie<double> w{3.0, 3.0, 3.0, 3.0, 3.0};
    EXPECT_THROW(df::stats::correlation(x, w),
                 std::runtime_error); // Zero std dev in w
    MSG("corelation throw ok");

    // Partial correlation
    df::Serie<double> p{1.0, 3.0, 2.0, 5.0, 4.0};
    double corr = df::stats::correlation(x, p);
    EXPECT_GT(corr, 0.0);
    EXPECT_LT(corr, 1.0);
    MSG("corelation partial ok");
}

TEST(Stats, PipelineOperations) {
    df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    // Test bind_mean
    auto mean_serie = values | df::stats::bind_mean<double>();
    EXPECT_EQ(mean_serie.size(), 1);
    EXPECT_NEAR(mean_serie[0], 5.0, 1e-10);

    // Test bind_variance
    auto var_serie = values | df::stats::bind_variance<double>();
    EXPECT_EQ(var_serie.size(), 1);
    EXPECT_NEAR(var_serie[0], 7.5, 1e-10);

    // Test bind_std_dev
    auto std_serie = values | df::stats::bind_std_dev<double>();
    EXPECT_EQ(std_serie.size(), 1);
    EXPECT_NEAR(std_serie[0], 2.7386127875258306, 1e-10);

    // Test bind_median
    auto median_serie = values | df::stats::bind_median<double>();
    EXPECT_EQ(median_serie.size(), 1);
    EXPECT_NEAR(median_serie[0], 5.0, 1e-10);

    // Test bind_quantile
    auto q1_serie = values | df::stats::bind_quantile<double>(0.25);
    EXPECT_EQ(q1_serie.size(), 1);
    EXPECT_NEAR(q1_serie[0], 3.0, 1e-10);

    // Test bind_z_score
    auto z_scores = values | df::stats::bind_z_score<double>();
    EXPECT_EQ(z_scores.size(), values.size());
    EXPECT_NEAR(df::stats::mean(z_scores), 0.0, 1e-10);
    EXPECT_NEAR(df::stats::std_dev(z_scores), 1.0, 1e-10);
}

// Further tests for edge cases and special scenarios
TEST(Stats, EdgeCases) {
    // Test with NaN values
    df::Serie<double> with_nan{1.0, 2.0,
                               std::numeric_limits<double>::quiet_NaN(), 4.0};
    EXPECT_TRUE(std::isnan(df::stats::mean(with_nan)));
    EXPECT_TRUE(std::isnan(df::stats::variance(with_nan)));
    EXPECT_TRUE(std::isnan(df::stats::std_dev(with_nan)));

    // Test with infinity
    df::Serie<double> with_inf{1.0, 2.0,
                               std::numeric_limits<double>::infinity(), 4.0};
    EXPECT_TRUE(std::isinf(df::stats::mean(with_inf)));
    EXPECT_TRUE(std::isnan(df::stats::variance(with_inf)) ||
                std::isinf(df::stats::variance(with_inf)));
    EXPECT_TRUE(std::isnan(df::stats::std_dev(with_inf)) ||
                std::isinf(df::stats::std_dev(with_inf)));
}

RUN_TESTS()