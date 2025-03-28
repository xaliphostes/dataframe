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
#include <dataframe/core/pipe.h>
#include <dataframe/stats/stats.h>
#include <limits>

using namespace df;

TEST(covariance, basic) {
    MSG("Testing basic covariance computation");

    // Perfect positive correlation
    Serie<double> s1{1.0, 2.0, 3.0, 4.0, 5.0};
    Serie<double> s2{2.0, 4.0, 6.0, 8.0, 10.0};
    EXPECT_NEAR(df::stats::covariance(s1, s2), 4.0, 1e-10);

    // Perfect negative correlation
    Serie<double> s3{5.0, 4.0, 3.0, 2.0, 1.0};
    EXPECT_NEAR(df::stats::covariance(s1, s3), -2.0, 1e-10);

    // No correlation (orthogonal)
    Serie<double> s4{2.0, 2.0, 2.0, 2.0, 2.0};
    EXPECT_NEAR(df::stats::covariance(s1, s4), 0.0, 1e-10);

    // Self covariance should equal variance
    EXPECT_NEAR(df::stats::covariance(s1, s1), 2.0, 1e-10);
}

TEST(covariance, sample) {
    MSG("Testing sample covariance computation");

    Serie<double> s1{1.0, 2.0, 3.0, 4.0, 5.0};
    Serie<double> s2{2.0, 4.0, 6.0, 8.0, 10.0};
}

TEST(covariance, edge_cases) {
    MSG("Testing covariance edge cases");

    // Empty series
    Serie<double> empty{};
    Serie<double> s1{1.0, 2.0, 3.0};
    EXPECT_THROW(df::stats::covariance(empty, empty), std::runtime_error);
    EXPECT_THROW(df::stats::covariance(s1, empty), std::runtime_error);

    // Different sizes
    Serie<double> s2{1.0, 2.0};
    EXPECT_THROW(df::stats::covariance(s1, s2), std::runtime_error);

    // Single element
    Serie<double> single1{1.0};
    Serie<double> single2{2.0};
    EXPECT_NO_THROW(df::stats::covariance(single1, single2));
}

TEST(covariance, integer) {
    MSG("Testing covariance with integer data");

    Serie<int> s1{1, 2, 3, 4, 5};
    Serie<int> s2{2, 4, 6, 8, 10};

    EXPECT_EQ(df::stats::covariance(s1, s2), 4);
}

// TEST(covariance, pipeline) {
//     MSG("Testing covariance pipeline operations");

//     Serie<double> s1{1.0, 2.0, 3.0, 4.0, 5.0};
//     Serie<double> s2{2.0, 4.0, 6.0, 8.0, 10.0};

//     // Test pipeline operation
//     auto result = s1 | df::stats::bind_covariance(s2);
//     EXPECT_NEAR(result, 4.0, 1e-10);
// }

RUN_TESTS()