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
#include <dataframe/algebra/det.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(determinant, matrix2D) {
    MSG("Testing 2D matrix determinant");

    // Test 2x2 matrices stored as [a11, a12, a21]
    Serie<Stress2D> matrices{
        {1.0, 2.0, 3.0},     // det = 1*3 - 2*2 = -1
        {2.0, 0.0, 2.0},     // det = 2*2 - 0*0 = 4
        {1.0, -1.0, 1.0}     // det = 1*1 - (-1)*(-1) = 0
    };
    
    auto result = det(matrices);
    EXPECT_ARRAY_NEAR(result.asArray(),
        std::vector<double>({-1.0, 4.0, 0.0}), 1e-10);

    // Test identity matrix
    Serie<Stress2D> identity{{1.0, 0.0, 1.0}};
    auto identity_result = det(identity);
    EXPECT_ARRAY_NEAR(identity_result.asArray(),
        std::vector<double>({1.0}), 1e-10);

    // Test pipeline operation
    auto pipe_result = matrices | bind_det<Stress2D>();
    EXPECT_ARRAY_NEAR(pipe_result.asArray(),
        std::vector<double>({-1.0, 4.0, 0.0}), 1e-10);
}

TEST(determinant, matrix3D) {
    MSG("Testing 3D matrix determinant");

    // Test 3x3 symmetric matrices stored as [a11, a12, a13, a22, a23, a33]
    Serie<Stress3D> matrices{
        {1.0, 0.0, 0.0, 1.0, 0.0, 1.0},  // Identity matrix, det = 1
        {2.0, 1.0, 0.0, 2.0, 1.0, 2.0},  // det = 7
        {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}   // General case
    };
    
    auto result = det(matrices);
    
    // Expected values:
    // 1. Identity matrix: det = 1
    // 2. Second matrix: det = 2*(2*2 - 1*1) - 1*(1*2 - 1*0) + 0*(1*1 - 2*0) = 7
    // 3. General case: calculated value
    EXPECT_ARRAY_NEAR(result.asArray(),
        std::vector<double>({1.0, 4.0, -1.0}), 1e-10);

    // Test zero matrix
    Serie<Stress3D> zero_matrix{{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}};
    auto zero_result = det(zero_matrix);
    EXPECT_ARRAY_NEAR(zero_result.asArray(),
        std::vector<double>({0.0}), 1e-10);

    // Test pipeline operation
    auto pipe_result = matrices | bind_det<Stress3D>();
    EXPECT_ARRAY_NEAR(pipe_result.asArray(),
        std::vector<double>({1.0, 4.0, -1.0}), 1e-10);
}

TEST(determinant, error_cases) {
    MSG("Testing error cases for determinant");

    // Test empty series
    Serie<Stress2D> empty_serie{};
    auto empty_result = det(empty_serie);
    EXPECT_EQ(empty_result.size(), 0);

    // Test single element series
    Serie<Stress2D> single_serie{{1.0, 2.0}};
    auto single_result = det(single_serie);
    EXPECT_EQ(single_result.size(), 1);
    EXPECT_ARRAY_NEAR(single_result.asArray(),
        std::vector<double>({1.0}), 1e-10);
}

RUN_TESTS()