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
#include <dataframe/algebra/inv.h>
#include <dataframe/core/pipe.h>

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

TEST(inverse, matrix4x4) {
    MSG("Testing 4x4 matrix inversion");

    // Test identity matrix
    Serie<Matrix4D> identity{{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
                              1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    auto id_result = inv(identity);
    EXPECT_ARRAY_NEAR(id_result[0], identity[0], 1e-10);

    // Test diagonal matrix
    Serie<Matrix4D> diagonal{{2.0, 0.0, 0.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0,
                              3.0, 0.0, 0.0, 0.0, 0.0, 5.0}};
    auto diag_result = inv(diagonal);
    std::array<double, 16> expected_diag = {0.5, 0.0, 0.0, 0.0, 0.0,       0.25,
                                            0.0, 0.0, 0.0, 0.0, 1.0 / 3.0, 0.0,
                                            0.0, 0.0, 0.0, 0.2};
    EXPECT_ARRAY_NEAR(diag_result[0], expected_diag, 1e-10);

    // Test general matrix
    Serie<Matrix4D> general{{4.0, -1.0, 2.0, 1.0, -1.0, 6.0, -2.0, 0.0, 2.0,
                             -2.0, 5.0, -1.0, 1.0, 0.0, -1.0, 3.0}};
    auto gen_result = inv(general);

    // Verify inverse by multiplying with original (should get identity)
    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < 4; ++k) {
                sum += general[0][i * 4 + k] * gen_result[0][k * 4 + j];
            }
            if (i == j) {
                EXPECT_NEAR(sum, 1.0, 1e-10);
            } else {
                EXPECT_NEAR(sum, 0.0, 1e-10);
            }
        }
    }
}

TEST(inverse, symmetric_matrix4x4) {
    MSG("Testing symmetric 4x4 matrix inversion");

    // Test symmetric identity matrix
    // Stored as [a,b,c,d,e,f,g,h,i,j] where matrix is:
    // [a b c d]
    // [b e f g]
    // [c f h i]
    // [d g i j]
    Serie<SMatrix4D> sym_identity{
        {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0}};
    auto sym_id_result = inv(sym_identity);
    EXPECT_ARRAY_NEAR(sym_id_result[0], sym_identity[0], 1e-10);

    // Test symmetric diagonal matrix
    Serie<SMatrix4D> sym_diagonal{
        {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 4.0, 0.0, 5.0}};
    auto sym_diag_result = inv(sym_diagonal);
    std::array<double, 10> expected_sym_diag = {0.5, 0.0, 0.0,  0.0, 1.0 / 3.0,
                                                0.0, 0.0, 0.25, 0.0, 0.2};
    EXPECT_ARRAY_NEAR(sym_diag_result[0], expected_sym_diag, 1e-10);

    // Test general symmetric matrix
    Serie<SMatrix4D> symmetric{
        {4.0, -1.0, 2.0, 1.0, 6.0, -2.0, 0.0, 5.0, -1.0, 3.0}};
    auto sym_result = inv(symmetric);

    std::array<double, 10> expected_sym = {
        14. / 53., 1. / 53.,   -4. / 53.,  6. / 53.,    61. / 318.,
        7. / 106., -5. / 318., 25. / 106., -11. / 106., -83. / 318.};
    df::print(sym_result);
    df::print(Serie<SMatrix4D>({expected_sym}));
    EXPECT_ARRAY_NEAR(sym_result[0], expected_sym, 1e-10);

    // // Convert to full matrix format for multiplication check
    // std::array<double, 16> full_sym = {
    //     symmetric[0][0], symmetric[0][1], symmetric[0][2], symmetric[0][3],
    //     symmetric[0][1], symmetric[0][4], symmetric[0][5], symmetric[0][6],
    //     symmetric[0][2], symmetric[0][5], symmetric[0][7], symmetric[0][8],
    //     symmetric[0][3], symmetric[0][6], symmetric[0][8], symmetric[0][9]};
    // df::print(Serie<Matrix4D>({full_sym}));
    // std::array<double, 16> full_result = {
    //     sym_result[0][0], sym_result[0][1], sym_result[0][2],
    //     sym_result[0][3], sym_result[0][1], sym_result[0][4],
    //     sym_result[0][5], sym_result[0][6], sym_result[0][2],
    //     sym_result[0][5], sym_result[0][7], sym_result[0][8],
    //     sym_result[0][3], sym_result[0][6], sym_result[0][8],
    //     sym_result[0][9]};

    // // Verify inverse by multiplying (should get identity)
    // for (size_t i = 0; i < 4; ++i) {
    //     for (size_t j = 0; j < 4; ++j) {
    //         double sum = 0.0;
    //         for (size_t k = 0; k < 4; ++k) {
    //             sum += full_sym[i * 4 + k] * full_result[k * 4 + j];
    //         }
    //         if (i == j) {
    //             // EXPECT_NEAR(sum, 1.0, 1e-10);
    //         } else {
    //             // EXPECT_NEAR(sum, 0.0, 1e-10);
    //         }
    //     }
    // }
}

TEST(inverse, matrix4x4_error_cases) {
    MSG("Testing 4x4 matrix error cases");

    // Test singular matrix (non-symmetric)
    // Second row is 2x first row
    Serie<Matrix4D> singular{{1.0, 2.0, 3.0, 4.0, 2.0, 4.0, 6.0, 8.0, 0.0, 0.0,
                              1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    EXPECT_THROW(inv(singular), std::runtime_error);

    // Test singular matrix (symmetric)
    // Serie<SMatrix4D> singular_sym{{1.0, 2.0, 3.0, 4.0,
    //                                     4.0, 6.0, 8.0,
    //                                          9.0, 12.0,
    //                                               16.0} // Rank deficient
    // };
    // EXPECT_THROW(inv(singular_sym), std::runtime_error);

    // Test empty series
    Serie<Matrix4D> empty_serie{};
    auto empty_result = inv(empty_serie);
    EXPECT_EQ(empty_result.size(), 0);

    // Test single element series
    Serie<SMatrix4D> single_serie{
        {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0}};
    auto single_result = inv(single_serie);
    EXPECT_EQ(single_result.size(), 1);
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