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

TEST(quantile, basic) {
    MSG("Testing basic quantile calculations");

    Serie<double> data{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    // Test specific quantiles
    EXPECT_NEAR(df::stats::quantile(data, 0.0), 1.0, 1e-10);
    EXPECT_NEAR(df::stats::quantile(data, 0.25), 3.0, 1e-10);
    EXPECT_NEAR(df::stats::quantile(data, 0.5), 5.0, 1e-10);
    EXPECT_NEAR(df::stats::quantile(data, 0.75), 7.0, 1e-10);
    EXPECT_NEAR(df::stats::quantile(data, 1.0), 9.0, 1e-10);

    // Test IQR
    EXPECT_NEAR(df::stats::iqr(data), 4.0, 1e-10);
}

TEST(quantile, outliers) {
    MSG("Testing outlier detection");

    // Dataset with clear outliers
    Serie<double> data{1.0, 2.0, 2.5, 2.7, 3.0, 3.2, 3.5, 4.0, 15.0, -5.0};

    const auto outliers = df::stats::isOutlier(data);
    const auto non_outliers = df::stats::notOutlier(data);

    // Verify outlier detection
    const std::vector<bool> expected_outliers = {
        false, false, false, false, false, false, false, false, true, true};
    EXPECT_ARRAY_EQ(outliers.asArray(), expected_outliers);

    // Verify non-outliers are complement of outliers
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(outliers[i], !non_outliers[i]);
    }
}

TEST(quantile, edge_cases) {
    MSG("Testing edge cases");

    // Empty serie
    Serie<double> empty{};
    EXPECT_THROW(df::stats::quantile(empty, 0.5), std::runtime_error);
    EXPECT_THROW(df::stats::iqr(empty), std::runtime_error);

    // Single value
    Serie<double> single{1.0};
    EXPECT_NO_THROW(df::stats::iqr(single));
}

TEST(quantile, integer) {
    MSG("Testing with integer data");

    Serie<int> data{1, 2, 3, 4, 5, 6, 7, 8, 9};

    EXPECT_EQ(df::stats::quantile(data, 0.25), 3);
    EXPECT_EQ(df::stats::quantile(data, 0.5), 5);
    EXPECT_EQ(df::stats::quantile(data, 0.75), 7);
    EXPECT_EQ(df::stats::iqr(data), 4);
}

TEST(quantile, pipeline) {
    MSG("Testing pipeline operations");

    Serie<double> data{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    // Test outlier pipeline
    const auto outliers = data | df::stats::bind_isOutlier<double>();
    const auto non_outliers = data | df::stats::bind_notOutlier<double>();

    // Verify pipeline results
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(outliers[i], !non_outliers[i]);
    }
}

RUN_TESTS()