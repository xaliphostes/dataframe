/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 */

#include "../../TEST.h"
#include <dataframe/algebra/eigen.h>
#include <dataframe/core/forEach.h>
#include <dataframe/core/pipe.h>
#include <dataframe/types.h>

using namespace df;

TEST(eigen_analysis, matrix2x2)
{
    MSG("Testing 2x2 symmetric matrix eigen decomposition");

    // Identity matrix - eigenvalues should be 1,1 and eigenvectors orthonormal
    {
        Serie<SMatrix2D> identity { { 1.0, 0.0, 1.0 } }; // [1 0; 0 1]
        auto id_values = eigenValues(identity);
        EXPECT_ARRAY_NEAR(id_values[0], std::vector<double>({ 1.0, 1.0 }), 1e-10);
    }

    // Diagonal matrix with distinct eigenvalues
    {
        Serie<SMatrix2D> diagonal { { 2.0, 0.0, 3.0 } }; // [2 0; 0 3]
        auto diag_values = eigenValues(diagonal);
        EXPECT_ARRAY_NEAR(diag_values[0], std::vector<double>({ 3.0, 2.0 }), 1e-10);
    }

    // General symmetric matrix
    {
        Serie<SMatrix2D> general { { 4.0, 1.0, 3.0 } }; // [4 1; 1 3]
        {
            auto values = eigenValues(general);
            EXPECT_ARRAY_NEAR(values[0], std::vector<double>({ 4.61803, 2.38197 }), 1e-5);

            auto vectors = eigenVectors(general);
            EXPECT_ARRAY_NEAR(vectors[0][0], std::vector<double>({ 0.850651, -0.525731 }), 1e-5);
            EXPECT_ARRAY_NEAR(vectors[0][1], std::vector<double>({ 0.525731, 0.850651 }), 1e-5);
        }
        {
            auto systems = eigenSystem(general);

            auto values = systems[0].first;
            EXPECT_ARRAY_NEAR(values, std::vector<double>({ 4.61803, 2.38197 }), 1e-5);

            auto vectors = systems[0].second;
            EXPECT_ARRAY_NEAR(vectors[0], std::vector<double>({ 0.850651, -0.525731 }), 1e-5);
            EXPECT_ARRAY_NEAR(vectors[1], std::vector<double>({ 0.525731, 0.850651 }), 1e-5);
        }
    }
}

TEST(eigen_analysis, matrix3x3)
{
    MSG("Testing 3x3 symmetric matrix eigen decomposition");
    Serie<SMatrix3D> identity { { 1.0, 0.0, 0.0, 1.0, 0.0, 1.0 } }; // [1 0 0; 0 1 0; 0 0 1]

    auto id_values = eigenValues(identity);
    EXPECT_ARRAY_NEAR(id_values[0], std::vector<double>({ 1.0, 1.0, 1.0 }), 1e-10);

    MSG("  Diagonal matrix");
    Serie<SMatrix3D> diagonal { { 2.0, 0.0, 0.0, 3.0, 0.0, 4.0 } }; // [2 0 0; 0 3 0; 0 0 4]

    auto diag_values = eigenValues(diagonal);
    EXPECT_ARRAY_NEAR(diag_values[0], std::vector<double>({ 4.0, 3.0, 2.0 }), 1e-10);

    MSG("  General symmetric matrix");
    df::Serie<SMatrix3D> serie(
        { { 2, 4, 6, 3, 6, 9 }, { 1, 2, 3, 4, 5, 6 }, { 9, 8, 7, 6, 5, 4 } });

    std::vector<Vector3> vals { { 16.3328, -0.658031, -1.67482 }, { 11.3448, 0.170914, -0.515728 },
        { 20.1911, -0.043142, -1.14795 } };

    auto values = df::eigenValues(serie);
    values.forEach(
        [vals](const auto& v, size_t index) { EXPECT_ARRAY_NEAR(v, vals[index], 1e-4); });

    // -------------------------------------------------

    std::vector<std::vector<std::vector<double>>> vecs
        = { { { 0.449308, 0.197038, 0.871377 }, { 0.475231, 0.77321, -0.419883 },
                { 0.756491, -0.602762, -0.253771 } },
              { { 0.327985, -0.591009, 0.736976 }, { 0.591009, 0.736976, 0.327985 },
                  { 0.736976, -0.327985, -0.591009 } },
              { { 0.688655, 0.159217, -0.707393 }, { 0.553323, -0.745891, 0.370783 },
                  { 0.468603, 0.646758, 0.60176 } } };

    auto vectors = df::eigenVectors(serie);
    vectors.forEach([vecs](const auto& m, size_t index) {
        for (size_t i = 0; i < 3; ++i) {
            EXPECT_ARRAY_NEAR(m[i], vecs[index][i], 1e-4);
        }
    });

    {
        auto systems = df::eigenSystem(serie);
        systems.forEach([](const auto& sv, size_t) {
            std::cout << sv.first << std::endl;
            std::cout << sv.second << std::endl;
        });
    }
}

// TEST(eigen_analysis, matrix4x4) {
//     MSG("Testing 4x4 symmetric matrix eigen decomposition");

//     // Identity matrix
//     Serie<SMatrix4D> identity{
//         {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0}};

//     auto id_values = eigenValues(identity);
//     EXPECT_ARRAY_NEAR(id_values[0],
//     std::vector<double>({1.0, 1.0, 1.0, 1.0}),
//                       1e-10);

//     // Diagonal matrix
//     Serie<SMatrix4D> diagonal{
//         {2.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 4.0, 0.0, 5.0}};

//     auto diag_values = eigenValues(diagonal);
//     EXPECT_ARRAY_NEAR(diag_values[0],
//     std::vector<double>({5.0, 4.0, 3.0, 2.0}),
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