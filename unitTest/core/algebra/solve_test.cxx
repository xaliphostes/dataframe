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
#include <dataframe/algebra/solve.h>

TEST(solve, simple_2x2) {
    // Create a 2x2 system:
    // 2x + y = 1
    // x + 3y = 2
    df::Serie<double> A{2.0, 1.0,  // First row
                        1.0, 3.0}; // Second row
    df::Serie<double> b{1.0, 2.0};

    auto x = df::solve(A, b);

    // Expected solution: x ≈ 0.2, y ≈ 0.6
    EXPECT_TRUE(x.size() == 2);
    EXPECT_NEAR(x[0], 0.2, 1e-10);
    EXPECT_NEAR(x[1], 0.6, 1e-10);
}

TEST(solve, identity_matrix) {
    df::Serie<double> A{1.0, 0.0, 0.0, 1.0};
    df::Serie<double> b{2.0, 3.0};

    auto x = df::solve(A, b);

    EXPECT_TRUE(x.size() == 2);
    EXPECT_NEAR(x[0], 2.0, 1e-10);
    EXPECT_NEAR(x[1], 3.0, 1e-10);
}

TEST(solve, non_square_matrix) {
    df::Serie<double> A{1.0, 2.0, 3.0}; // 1x3 matrix
    df::Serie<double> b{1.0};

    EXPECT_THROW(df::solve(A, b), std::invalid_argument);
}

TEST(solve, size_mismatch) {
    df::Serie<double> A{1.0, 0.0, 0.0, 1.0}; // 2x2 matrix
    df::Serie<double> b{1.0, 2.0, 3.0};      // 3x1 vector

    EXPECT_THROW(df::solve(A, b), std::invalid_argument);
}

TEST(solve, larger_system) {
    // 3x3 system
    df::Serie<double> A{4.0, -1.0, 0.0, -1.0, 4.0, -1.0, 0.0, -1.0, 4.0};
    df::Serie<double> b{1.0, 5.0, 2.0};

    auto x = df::solve(A, b);

    EXPECT_TRUE(x.size() == 3);

    // Verify solution by multiplying Ax
    df::Serie<double> Ax(3);
    for (size_t i = 0; i < 3; ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < 3; ++j) {
            sum += A[i * 3 + j] * x[j];
        }
        Ax[i] = sum;
    }

    for (size_t i = 0; i < 3; ++i) {
        EXPECT_NEAR(Ax[i], b[i], 1e-10);
    }
}

TEST(solve, pipe_operator) {
    df::Serie<double> A{2.0, 1.0, 1.0, 3.0};
    df::Serie<double> b{1.0, 2.0};

    auto x = A | df::bind_solve(b);

    EXPECT_TRUE(x.size() == 2);
    EXPECT_NEAR(x[0], 0.2, 1e-10);
    EXPECT_NEAR(x[1], 0.6, 1e-10);
}

RUN_TESTS();