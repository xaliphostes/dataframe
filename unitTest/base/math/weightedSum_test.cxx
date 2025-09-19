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
#include <array>
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/math/add.h>
#include <dataframe/math/div.h>
#include <dataframe/math/mult.h>
#include <dataframe/math/sub.h>
#include <dataframe/math/weightedSum2.h>

using namespace df;

TEST(weightedSum, scalar)
{
    MSG("Testing weighted sum with scalar weights");

    // Test integer weighted sum
    df::Serie<int> s1 { 1, 2, 3 };
    df::Serie<int> s2 { 4, 5, 6 };
    df::Serie<int> s3 { 7, 8, 9 };
    std::vector<int> w { 2, 3, 4 };
    auto result = df::weightedSum({ s1, s2, s3 }, w);
    // Expected: 2*(1,2,3) + 3*(4,5,6) + 4*(7,8,9)
    //           = (2,4,6) + (12,15,18) + (28,32,36)
    //           = (42,51,60)
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<int>({ 42, 51, 60 }));

    // Test float weighted sum
    df::Serie<float> f1 { 1.0f, 2.0f, 3.0f };
    df::Serie<float> f2 { 0.5f, 1.5f, 2.5f };
    auto float_result = df::weightedSum<float>({ f1, f2 }, { 0.5f, 1.5f });
    EXPECT_ARRAY_NEAR(float_result.asArray(), std::vector<float>({ 1.25f, 3.25f, 5.25f }), 1e-6f);

    // Test error on size mismatch between series and weights
    EXPECT_THROW(df::weightedSum<int>({ s1, s2 }, { 1, 2, 3 }), std::runtime_error);

    // Test error on size mismatch between series
    df::Serie<int> s4 { 1, 2 };
    EXPECT_THROW(df::weightedSum<int>({ s1, s4 }, { 1, 2 }), std::runtime_error);

    // Test empty series list
    // auto empty_result = df::weightedSum<int>({}, {});
    // EXPECT_EQ(empty_result.size(), 0);

    // Test pipeline operation
    // auto result_pipe = s1 | df::bind_weightedSum<int>({s2, s3}, std::vector<int>{2, 3, 4});
    // EXPECT_ARRAY_EQ(result_pipe.asArray(), std::vector<int>({42, 51, 60}));
}

TEST(weightedSum, serie)
{
    MSG("Testing weighted sum with series weights");

    df::Serie<double> s1 { 1.0, 2.0, 3.0 };
    df::Serie<double> s2 { 4.0, 5.0, 6.0 };
    df::Serie<double> w1 { 0.5, 1.0, 1.5 };
    df::Serie<double> w2 { 1.5, 1.0, 0.5 };

    // auto result = df::weightedSum<double>(std::vector<df::Serie<double>>({s1, s2}), {w1, w2});
    auto result = df::weightedSum<double>({ s1, s2 }, { w1, w2 });
    // Expected: (1,2,3)*(0.5,1.0,1.5) + (4,5,6)*(1.5,1.0,0.5)
    EXPECT_ARRAY_NEAR(result.asArray(), std::vector<double>({ 6.5, 7.0, 7.5 }), 1e-6);

    // Test error on size mismatch between series and weights
    df::Serie<double> w3 { 0.5, 1.0 };
    EXPECT_THROW(df::weightedSum<double>({ s1, s2 }, { w1, w3 }), std::runtime_error);

    // Test error on size mismatch between number of series and weights
    EXPECT_THROW(df::weightedSum<double>({ s1, s2 }, { w1 }), std::runtime_error);

    // Test empty series list
    auto empty_result = df::weightedSum<double>({}, std::vector<double>());
    EXPECT_EQ(empty_result.size(), 0);

    // Test pipeline operation
    auto result_pipe = s1 | df::bind_weightedSum<double>({ s2 }, { w1, w2 });
    EXPECT_ARRAY_NEAR(result_pipe.asArray(), std::vector<double>({ 6.5, 7.0, 7.5 }), 1e-6);
}

TEST(weightedSum, geol)
{
    // Create 4 stress tensor measurements from different locations/times
    // Each tensor has format [xx, xy, xz, yy, yz, zz]

    // Stress measurement 0: Normal faulting regime
    df::Serie<SMatrix3D> S1({ { -80.0, 2.0, 1.0, -60.0, 3.0, -20.0 },
        { -75.0, 1.5, 0.8, -55.0, 2.8, -18.0 }, { -70.0, 1.0, 0.5, -50.0, 2.5, -15.0 } });

    // Stress measurement 1: Strike-slip regime
    df::Serie<SMatrix3D> S2({ { -60.0, 15.0, 8.0, -40.0, 12.0, -30.0 },
        { -58.0, 14.0, 7.5, -38.0, 11.5, -28.0 }, { -55.0, 13.0, 7.0, -35.0, 11.0, -25.0 } });

    // Stress measurement 2: Thrust faulting regime
    df::Serie<SMatrix3D> S3({ { -30.0, 5.0, 3.0, -50.0, 8.0, -80.0 },
        { -28.0, 4.8, 2.8, -48.0, 7.8, -78.0 }, { -25.0, 4.5, 2.5, -45.0, 7.5, -75.0 } });

    // Stress measurement 3: Mixed regime with measurement uncertainty
    df::Serie<SMatrix3D> S4({ { -65.0, 8.0, 4.0, -45.0, 6.0, -35.0 },
        { -62.0, 7.5, 3.8, -43.0, 5.8, -33.0 }, { -60.0, 7.0, 3.5, -40.0, 5.5, -30.0 } });

    df::Serie<SMatrix3D> expectedStress({ { -60.250, 6.900, 3.700, -50.250, 6.950, -39.750 },
        { -57.050, 6.350, 3.425, -47.200, 6.675, -37.750 },
        { -53.500, 5.775, 3.075, -43.500, 6.325, -34.750 } });

    // Compute weighted average stress tensor from the dataframe
    auto avgStress1 = df::weightedSum({ S1, S2, S3, S4 }, { 0.35, 0.25, 0.25, 0.15 });
}

// ----------------------------------------------------------

using Vector3D = std::array<double, 3>;

// Helper function for vector comparisons
void EXPECT_VECTOR3D_NEAR(const Vector3D& v1, const Vector3D& v2, double tol)
{
    for (size_t i = 0; i < 3; ++i) {
        if (std::abs(v1[i] - v2[i]) > tol) {
            std::stringstream ss;
            ss << "Vectors differ at component " << i << ": " << v1[i] << " != " << v2[i]
               << " (diff = " << std::abs(v1[i] - v2[i]) << ", tolerance = " << tol << ")";
            throw std::runtime_error(ss.str());
        }
    }
}

void EXPECT_SERIE_VECTOR3D_NEAR(
    const df::Serie<Vector3D>& s1, const df::Serie<Vector3D>& s2, double tol)
{
    if (s1.size() != s2.size()) {
        throw std::runtime_error("Series have different sizes");
    }
    for (size_t i = 0; i < s1.size(); ++i) {
        EXPECT_VECTOR3D_NEAR(s1[i], s2[i], tol);
    }
}

/*
TEST(Serie, Vector3DWeightedSum)
{
    MSG("Testing Vector3D weighted sum");

    df::Serie<Vector3D> s1{Vector3D{1.0, 0.0, 0.0}, Vector3D{0.0, 1.0, 0.0}};

    df::Serie<Vector3D> s2{Vector3D{0.0, 1.0, 0.0}, Vector3D{1.0, 0.0, 1.0}};

    // Test with scalar weights
    auto weighted = df::weightedSum<Vector3D>({s1, s2}, {2.0, 0.5});

    Serie<Vector3D> expected{Vector3D{2.0, 0.5, 0.0}, Vector3D{0.0, 2.0, 0.5}};

    EXPECT_SERIE_VECTOR3D_NEAR(weighted, expected, 1e-10);

    // // Test with series weights
    // df::Serie<double> w1{1.0, 0.5};
    // df::Serie<double> w2{0.5, 1.0};

    // auto weighted_series = df::weightedSum<Vector3D>({s1, s2}, {w1, w2});

    // df::Serie<Vector3D> expected_series{Vector3D{1.0, 0.5, 0.0},
    //                                     Vector3D{0.5, 0.5, 1.0}};

    // EXPECT_SERIE_VECTOR3D_NEAR(weighted_series, expected_series, 1e-10);

    // Test pipeline operation
    // auto result_pipe = s1 | bind_weightedSum<Vector3D>({s2}, {2.0, 0.5});
    // EXPECT_SERIE_VECTOR3D_NEAR(result_pipe, expected, 1e-10);
}
*/

RUN_TESTS()