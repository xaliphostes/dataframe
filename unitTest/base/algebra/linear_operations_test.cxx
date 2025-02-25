/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 */

#include "../../TEST.h"
#include <dataframe/algebra/cross.h>
#include <dataframe/algebra/dot.h>
#include <dataframe/algebra/norm.h>
#include <dataframe/algebra/transpose.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(cross_product, vector3D) {
    MSG("Testing 3D vector cross product");

    // Test basic cross products
    Serie<Vector3> vectors1{{1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
    Serie<Vector3> vectors2{{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

    auto result = cross(vectors1, vectors2);
    EXPECT_ARRAY_NEAR(result[0], std::vector<double>({0.0, 0.0, 1.0}), 1e-10);
    EXPECT_ARRAY_NEAR(result[1], std::vector<double>({0.0, -1.0, 0.0}), 1e-10);

    // Test zero vectors
    Serie<Vector3> zero_vectors{{0.0, 0.0, 0.0}};
    auto zero_result = cross(zero_vectors, zero_vectors);
    EXPECT_ARRAY_NEAR(zero_result[0], std::vector<double>({0.0, 0.0, 0.0}),
                      1e-10);

    // Test pipeline operation
    auto pipe_result = vectors1 | bind_cross<double>(vectors2);
    EXPECT_ARRAY_NEAR(pipe_result[0], std::vector<double>({0.0, 0.0, 1.0}),
                      1e-10);

    // Test different sized series
    Serie<Vector3> single_vec{{1.0, 0.0, 0.0}};
    EXPECT_THROW(cross(vectors1, single_vec), std::runtime_error);
}

TEST(dot_product, vectors) {
    MSG("Testing vector dot product");

    // Test 2D vectors
    Serie<Vector2> vec2d_1{{1.0, 0.0}, {1.0, 1.0}};
    Serie<Vector2> vec2d_2{{0.0, 1.0}, {1.0, 1.0}};

    auto result2d = dot(vec2d_1, vec2d_2);
    EXPECT_ARRAY_NEAR(result2d.asArray(), std::vector<double>({0.0, 2.0}),
                      1e-10);

    // Test 3D vectors
    Serie<Vector3> vec3d_1{{1.0, 0.0, 0.0}, {1.0, 1.0, 1.0}};
    Serie<Vector3> vec3d_2{{1.0, 0.0, 0.0}, {2.0, 2.0, 2.0}};

    auto result3d = dot(vec3d_1, vec3d_2);
    EXPECT_ARRAY_NEAR(result3d.asArray(), std::vector<double>({1.0, 6.0}),
                      1e-10);

    // Test pipeline operation
    auto pipe_result = vec3d_1 | bind_dot<double, 3>(vec3d_2);
    EXPECT_ARRAY_NEAR(pipe_result.asArray(), std::vector<double>({1.0, 6.0}),
                      1e-10);

    // Test different sized series
    Serie<Vector3> single_vec{{1.0, 0.0, 0.0}};
    EXPECT_THROW(dot(vec3d_1, single_vec), std::runtime_error);
}

TEST(vector_norm, various_dimensions) {
    MSG("Testing vector norm calculation");

    // Test 2D vectors
    Serie<Vector2> vec2d{{3.0, 4.0}, {0.0, 1.0}};
    auto result2d = norm(vec2d);
    EXPECT_ARRAY_NEAR(result2d.asArray(), std::vector<double>({5.0, 1.0}),
                      1e-10);

    // Test 3D vectors
    Serie<Vector3> vec3d{{1.0, 2.0, 2.0}, {0.0, 0.0, 0.0}};
    auto result3d = norm(vec3d);
    EXPECT_ARRAY_NEAR(result3d.asArray(), std::vector<double>({3.0, 0.0}),
                      1e-10);

    // Test pipeline operation
    auto pipe_result = vec2d | bind_norm<double, 2>();
    EXPECT_ARRAY_NEAR(pipe_result.asArray(), std::vector<double>({5.0, 1.0}),
                      1e-10);

    // Test empty series
    Serie<Vector2> empty_serie{};
    auto empty_result = norm(empty_serie);
    EXPECT_EQ(empty_result.size(), 0);
}

TEST(matrix_transpose, various_sizes) {
    MSGD("Testing matrix transpose");

    MSGD("Test transpose 2x2 matrices");
    {
        Serie<Matrix2D> mat{{1.0, 2.0, 3.0, 4.0}, {0.0, 1.0, -1.0, 0.0}};
        Serie<Matrix2D> sol{{1.0, 3.0, 2.0, 4.0}, {0.0, -1.0, 1.0, 0.0}};

        auto res = transpose(mat);
        EXPECT_ARRAY_NEAR(res[0], sol[0], 1e-10);
        EXPECT_ARRAY_NEAR(res[1], sol[1], 1e-10);

        res = transpose(res);
        EXPECT_ARRAY_NEAR(res[0], mat[0], 1e-10);
        EXPECT_ARRAY_NEAR(res[1], mat[1], 1e-10);
    }

    MSGD("Test transpose 3x3 matrices");
    {
        Serie<Matrix3D> mat{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}};
        Serie<Matrix3D> sol{{1.0, 4.0, 7.0, 2.0, 5.0, 8.0, 3.0, 6.0, 9.0}};

        auto res = transpose(mat);
        EXPECT_ARRAY_NEAR(res[0], sol[0], 1e-10);

        res = transpose(res);
        EXPECT_ARRAY_NEAR(res[0], mat[0], 1e-10);
    }

    MSGD("Test transpose 4x4 matrices");
    {
        Serie<Matrix4D> mat{{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                             11.0, 12.0, 13.0, 14.0, 15.0, 16.0}};
        Serie<Matrix4D> sol{{1.0, 5.0, 9.0, 13.0, 2.0, 6.0, 10.0, 14.0, 3.0,
                             7.0, 11.0, 15.0, 4.0, 8.0, 12.0, 16.0}};
        auto res = transpose(mat);

        EXPECT_ARRAY_NEAR(res[0], sol[0], 1e-10);

        res = transpose(res);
        EXPECT_ARRAY_NEAR(res[0], mat[0], 1e-10);
    }

    // Test pipeline operation
    {
        Serie<Matrix2D> mat2d{{1.0, 2.0, 3.0, 4.0}, {0.0, 1.0, -1.0, 0.0}};

        auto pipe_result = mat2d | bind_transpose<double, 4>();
        EXPECT_ARRAY_NEAR(pipe_result[0],
                          std::vector<double>({1.0, 3.0, 2.0, 4.0}), 1e-10);
    }

    // Test empty series
    Serie<Matrix2D> empty_serie{};
    auto empty_result = transpose(empty_serie);
    EXPECT_EQ(empty_result.size(), 0);
}

RUN_TESTS()