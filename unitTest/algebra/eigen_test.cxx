/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 */

#include "../TEST.h"
#include <dataframe/algebra/eigen.h>
#include <dataframe/pipe.h>
#include <dataframe/types.h>
#include <dataframe/forEach.h>

using namespace df;

TEST(eigen_analysis, matrix2x2) {
    MSG("Testing 2x2 symmetric matrix eigen decomposition");

    // Identity matrix - eigenvalues should be 1,1 and eigenvectors orthonormal
    Serie<SMatrix2D> identity{{1.0, 0.0, 1.0}}; // [1 0; 0 1]

    auto id_values = eigenValues(identity);
    EXPECT_ARRAY_NEAR(id_values[0], std::vector<double>({1.0, 1.0}), 1e-10);

    auto id_vectors = eigenVectors(identity);

    // Check orthogonality of eigenvectors
    // double dot_product = id_vectors[0][0] * id_vectors[0][1] +
    //                      id_vectors[0][1] * id_vectors[0][2];
    // EXPECT_NEAR(dot_product, 0.0, 1e-10);

    // Diagonal matrix with distinct eigenvalues
    Serie<SMatrix2D> diagonal{{2.0, 0.0, 3.0}}; // [2 0; 0 3]

    auto diag_values = eigenValues(diagonal);
    EXPECT_ARRAY_NEAR(diag_values[0], std::vector<double>({3.0, 2.0}), 1e-10);

    // General symmetric matrix
    Serie<SMatrix2D> general{{4.0, 1.0, 3.0}}; // [4 1; 1 3]

    auto [vectors, values] = eigenSystem(general);

    // Verify eigendecomposition: A*v = lambda*v
    for (size_t i = 0; i < 2; ++i) {
        double left_x = 4.0 * vectors[0][0] + 1.0 * vectors[0][1];
        double left_y = 1.0 * vectors[0][0] + 3.0 * vectors[0][2];
        double right_x = values[0][i] * vectors[0][0];
        double right_y = values[0][i] * vectors[0][1];
        EXPECT_NEAR(left_x, right_x, 1e-10);
        EXPECT_NEAR(left_y, right_y, 1e-10);
    }
}

TEST(eigen_analysis, matrix3x3) {
    MSG("Testing 3x3 symmetric matrix eigen decomposition");

    MSG("  Identity matrix");
    Serie<SMatrix3D> identity{
        {1.0, 0.0, 0.0, 1.0, 0.0, 1.0}}; // [1 0 0; 0 1 0; 0 0 1]

    auto id_values = eigenValues(identity);
    df::print(id_values);
    // EXPECT_ARRAY_NEAR(id_values[0], std::vector<double>({1.0, 1.0, 1.0}),
    //                   1e-10);

    MSG("  Diagonal matrix");
    Serie<SMatrix3D> diagonal{
        {2.0, 0.0, 0.0, 3.0, 0.0, 4.0}}; // [2 0 0; 0 3 0; 0 0 4]

    auto diag_values = eigenValues(diagonal);
    df::print(diag_values);
    // EXPECT_ARRAY_NEAR(diag_values[0], std::vector<double>({4.0, 3.0, 2.0}),
    //                   1e-10);

    MSG("  General symmetric matrix");
    df::Serie<SMatrix3D> serie(
        {{2, 4, 6, 3, 6, 9}, {1, 2, 3, 4, 5, 6}, {9, 8, 7, 6, 5, 4}});

    std::vector<Vector3> vals{{16.3328, -0.658031, -1.67482},
                              {11.3448, 0.170914, -0.515728},
                              {20.1911, -0.043142, -1.14795}};

    // std::vector<Array> vecs{{0.449309, 0.47523, 0.75649, 0.194453, 0.774452,
    //                          -0.602007, 0.871957, -0.417589, -0.255559},
    //                         {0.327985, 0.591009, 0.736977, -0.592113, 0.736484,
    //                          -0.327099, 0.73609, 0.32909, -0.5915},
    //                         {0.688783, 0.553441, 0.468275, 0.15941, -0.745736,
    //                          0.64689, -0.707225, 0.370919, 0.601874}};

    {
        auto values = df::eigenValues(serie);
        auto vectors = df::eigenVectors(serie);

        df::print(values);

        df::forEach([](const EigenVectorType<3>& v, size_t) {
            std::cout << "1st eigen vector: " << v[0] << std::endl ;
            std::cout << "2nd eigen vector: " << v[1] << std::endl ;
            std::cout << "3rd eigen vector: " << v[2] << std::endl ;
        }, vectors);
    }

    {
        auto [values, vectors] = df::eigenSystem(serie);
        df::print(values);
        df::print(vectors);
    }
}

// TEST(eigen_analysis, matrix4x4) {
//     MSG("Testing 4x4 symmetric matrix eigen decomposition");

//     // Identity matrix
//     Serie<SMatrix4D> identity{
//         {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0}};

//     auto id_values = eigenValues(identity);
//     EXPECT_ARRAY_NEAR(id_values[0], std::vector<double>({1.0, 1.0, 1.0, 1.0}),
//                       1e-10);

//     // Diagonal matrix
//     Serie<SMatrix4D> diagonal{
//         {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 4.0, 0.0, 5.0}};

//     auto diag_values = eigenValues(diagonal);
//     EXPECT_ARRAY_NEAR(diag_values[0], std::vector<double>({5.0, 4.0, 3.0, 2.0}),
//                       1e-10);

//     // General symmetric matrix
//     Serie<SMatrix4D> general{
//         {4.0, 1.0, 0.0, 2.0, 3.0, 2.0, 1.0, 5.0, 0.0, 6.0}};

//     auto [vectors, values] = eigenSystem(general);

//     // Verify eigenvectors are normalized
//     for (size_t i = 0; i < 4; ++i) {
//         double norm = 0.0;
//         for (size_t j = 0; j < 4; ++j) {
//             norm += vectors[0][j] * vectors[0][j];
//         }
//         EXPECT_NEAR(norm, 1.0, 1e-10);
//     }
// }

// TEST(eigen_analysis, error_cases) {
//     MSG("Testing eigen analysis error cases");

//     // Empty series
//     Serie<SMatrix3D> empty_serie{};

//     auto empty_values = eigenValues(empty_serie);
//     EXPECT_EQ(empty_values.size(), 0);

//     auto empty_vectors = eigenVectors(empty_serie);
//     EXPECT_EQ(empty_vectors.size(), 0);

//     auto [empty_sys_vec, empty_sys_val] = eigenSystem(empty_serie);
//     EXPECT_EQ(empty_sys_vec.size(), 0);
//     EXPECT_EQ(empty_sys_val.size(), 0);

//     // Test pipeline operations
//     Serie<SMatrix2D> pipe_test{{1.0, 0.0, 1.0}};
//     auto pipe_result = pipe_test | bind_eigenValues<double, 3>();
//     EXPECT_EQ(pipe_result.size(), 1);
// }

RUN_TESTS()