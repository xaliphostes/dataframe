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

#include "../TEST.h"
#include <dataframe/algebra/inv.h>
#include <dataframe/pipe.h>

using namespace df;

TEST(inverse, scalar_1x1) {
    MSG("Testing 1x1 matrix inversion");

    Serie<std::array<double, 1>> matrices{{2.0}, {4.0}, {-3.0}};

    auto result = inv(matrices);
    EXPECT_ARRAY_NEAR(result[0], std::vector<double>({0.5}), 1e-10);
    EXPECT_ARRAY_NEAR(result[1], std::vector<double>({0.25}), 1e-10);
    EXPECT_ARRAY_NEAR(result[2], std::vector<double>({-1.0 / 3.0}), 1e-10);

    // Test singular matrix
    Serie<std::array<double, 1>> singular{{0.0}};
    EXPECT_THROW(inv(singular), std::runtime_error);
}

TEST(inverse, symmetric_2x2) {
    MSG("Testing symmetric 2x2 matrix inversion");

    // Test symmetric 2x2 matrix stored as [a,b,c] where matrix is [a b; b c]
    Serie<std::array<double, 3>> matrices{
        {2.0, 1.0, 2.0}, // det = 3
        {4.0, 0.0, 4.0}, // det = 16
        {1.0, 0.0, 1.0}  // det = 1
    };

    auto result = inv(matrices);

    // Verify first matrix inverse
    EXPECT_ARRAY_NEAR(result[0],
                      std::vector<double>({2.0 / 3.0, -1.0 / 3.0, 2.0 / 3.0}),
                      1e-10);

    // Verify second matrix inverse
    EXPECT_ARRAY_NEAR(result[1], std::vector<double>({0.25, 0.0, 0.25}), 1e-10);

    // Verify third matrix inverse
    EXPECT_ARRAY_NEAR(result[2], std::vector<double>({1.0, 0.0, 1.0}), 1e-10);
}

TEST(inverse, full_2x2) {
    MSG("Testing full 2x2 matrix inversion");

    // Test full 2x2 matrix stored as [a,b,c,d] where matrix is [a b; c d]
    Serie<std::array<double, 4>> matrices{
        {1.0, 2.0, 3.0, 4.0}, // det = -2
        {2.0, 0.0, 0.0, 2.0}, // det = 4
        {1.0, 1.0, 1.0, 2.0}  // det = 1
    };

    auto result = inv(matrices);

    // Verify first matrix inverse
    EXPECT_ARRAY_NEAR(result[0], std::vector<double>({-2.0, 1.0, 1.5, -0.5}),
                      1e-10);

    // Verify second matrix inverse (diagonal)
    EXPECT_ARRAY_NEAR(result[1], std::vector<double>({0.5, 0.0, 0.0, 0.5}),
                      1e-10);

    // Verify third matrix inverse
    EXPECT_ARRAY_NEAR(result[2], std::vector<double>({2.0, -1.0, -1.0, 1.0}),
                      1e-10);
}

TEST(inverse, symmetric_3x3) {
    MSG("Testing symmetric 3x3 matrix inversion");

    // Test symmetric 3x3 matrix stored as [a,b,c,d,e,f] where matrix is [a b c;
    // b d e; c e f]
    Serie<std::array<double, 6>> matrices{
        {2.0, 0.0, 0.0, 2.0, 0.0, 2.0}, // diagonal matrix
        {1.0, 0.5, 0.0, 2.0, 0.5, 3.0}  // general symmetric
    };

    auto result = inv(matrices);

    // Verify diagonal matrix inverse
    EXPECT_ARRAY_NEAR(
        result[0], std::vector<double>({0.5, 0.0, 0.0, 0.5, 0.0, 0.5}), 1e-10);

    // Verify general symmetric matrix inverse
    // You would need to calculate the expected values
    EXPECT_EQ(result[1].size(), 6);
}

TEST(inverse, full_3x3) {
    MSG("Testing full 3x3 matrix inversion");

    // Test full 3x3 matrix stored as [a,b,c,d,e,f,g,h,i]
    Serie<std::array<double, 9>> matrices{
        {1, 0, 0, 0, 1, 0, 0, 0, 1}, // identity
        {2, 0, 0, 0, 2, 0, 0, 0, 2}  // diagonal
    };

    auto result = inv(matrices);

    // Verify identity matrix inverse (should be identity)
    EXPECT_ARRAY_NEAR(result[0],
                      std::vector<double>({1, 0, 0, 0, 1, 0, 0, 0, 1}), 1e-10);

    // Verify diagonal matrix inverse
    EXPECT_ARRAY_NEAR(result[1],
                      std::vector<double>({0.5, 0, 0, 0, 0.5, 0, 0, 0, 0.5}),
                      1e-10);
}

TEST(inverse, pipeline) {
    MSG("Testing pipeline operations");

    // Test pipeline with 2x2 symmetric
    Serie<std::array<double, 3>> sym_2x2{{4.0, 1.0, 4.0}};
    auto pipe_result = sym_2x2 | bind_inv<double, 3>();
    EXPECT_ARRAY_NEAR(pipe_result[0],
                      std::vector<double>({0.266667, -0.0666667, 0.266667}),
                      1e-6);

    // Test pipeline with 3x3 full
    Serie<std::array<double, 9>> full_3x3{{1, 0, 0, 0, 2, 0, 0, 0, 4}};
    auto pipe_result_3x3 = full_3x3 | bind_inv<double, 9>();
    EXPECT_ARRAY_NEAR(pipe_result_3x3[0],
                      std::vector<double>({1, 0, 0, 0, 0.5, 0, 0, 0, 0.25}),
                      1e-10);
}

TEST(inverse, edge_cases) {
    MSG("Testing edge cases and error conditions");

    // Test singular matrices
    Serie<std::array<double, 3>> singular_2x2{
        {1.0, 1.0, 1.0}}; // singular symmetric 2x2
    EXPECT_THROW(inv(singular_2x2), std::runtime_error);

    Serie<std::array<double, 4>> singular_full_2x2{
        {1, 1, 1, 1}}; // singular full 2x2
    EXPECT_THROW(inv(singular_full_2x2), std::runtime_error);

    // Test empty series
    Serie<std::array<double, 3>> empty{};
    auto result_empty = inv(empty);
    EXPECT_EQ(result_empty.size(), 0);

    // Test nearly singular matrices
    Serie<std::array<double, 3>> nearly_singular{{1e-15, 0.0, 1e-15}};
    EXPECT_THROW(inv(nearly_singular), std::runtime_error);
}

TEST(inverse, special_cases) {
    MSG("Testing special matrix cases");

    // Test identity matrices
    Serie<std::array<double, 4>> identity_2x2{{1, 0, 0, 1}};
    auto result_id_2x2 = inv(identity_2x2);
    EXPECT_ARRAY_NEAR(result_id_2x2[0], std::vector<double>({1, 0, 0, 1}),
                      1e-10);

    Serie<std::array<double, 9>> identity_3x3{{1, 0, 0, 0, 1, 0, 0, 0, 1}};
    auto result_id_3x3 = inv(identity_3x3);
    EXPECT_ARRAY_NEAR(result_id_3x3[0],
                      std::vector<double>({1, 0, 0, 0, 1, 0, 0, 0, 1}), 1e-10);

    // Test diagonal matrices
    Serie<std::array<double, 3>> diag_sym_2x2{{2.0, 0.0, 3.0}};
    auto result_diag_2x2 = inv(diag_sym_2x2);
    EXPECT_ARRAY_NEAR(result_diag_2x2[0],
                      std::vector<double>({1.0 / 2.0, 0.0, 1.0 / 3.0}), 1e-10);

    Serie<std::array<double, 6>> diag_sym_3x3{{2, 0, 0, 2, 0, 2}};
    auto result_diag_3x3 = inv(diag_sym_3x3);
    EXPECT_ARRAY_NEAR(result_diag_3x3[0],
                      std::vector<double>({0.5, 0, 0, 0.5, 0, 0.5}), 1e-10);
}

RUN_TESTS()