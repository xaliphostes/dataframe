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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "TEST.h"
#include <dataframe/functional/geo/insar.h>

TEST(insar, basic) {
    // Create a displacement field (3D vectors)
    df::GenSerie<double> u(3,
                           {
                               1.0, 0.0, 0.0, // Point 1: displacement along x
                               0.0, 2.0, 0.0, // Point 2: displacement along y
                               0.0, 0.0, 3.0  // Point 3: displacement along z
                           });

    // Test with different line-of-sight vectors
    std::vector<double> los_x = {1.0, 0.0, 0.0}; // Looking along x
    auto result_x = df::geo::insar(u, los_x);
    EXPECT_EQ(result_x.itemSize(), 1);
    EXPECT_EQ(result_x.count(), 3);
    EXPECT_NEAR(result_x.value(0), 1.0, 1e-10); // Full x component
    EXPECT_NEAR(result_x.value(1), 0.0, 1e-10); // No y component
    EXPECT_NEAR(result_x.value(2), 0.0, 1e-10); // No z component

    // Looking at 45 degrees between x and y
    std::vector<double> los_xy = {1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0),
                                  0.0};
    auto result_xy = df::geo::insar(u, los_xy);
    EXPECT_NEAR(result_xy.value(0), 1.0 / std::sqrt(2.0),
                1e-10); // x projection
    EXPECT_NEAR(result_xy.value(1), 2.0 / std::sqrt(2.0),
                1e-10);                          // y projection
    EXPECT_NEAR(result_xy.value(2), 0.0, 1e-10); // no z projection
}

TEST(insar, error_cases) {
    // Test with wrong itemSize
    df::GenSerie<double> wrong_size(2, {1.0, 0.0, 1.0, 0.0});
    std::vector<double> los = {1.0, 0.0, 0.0};
    auto result1 = df::geo::insar(wrong_size, los);
    EXPECT_EQ(result1.count(), 0); // Should return empty serie

    // Test with wrong LOS vector size
    df::GenSerie<double> u(3, {1.0, 0.0, 0.0});
    std::vector<double> wrong_los = {1.0, 0.0};
    auto result2 = df::geo::insar(u, wrong_los);
    EXPECT_EQ(result2.count(), 0); // Should return empty serie
}

TEST(fringes, basic) {
    // Create a simple insar signal
    df::GenSerie<double> insar(1, {
                                      0.0, // No displacement
                                      2.5, // 2.5 units displacement
                                      5.0, // 5.0 units displacement
                                      7.5  // 7.5 units displacement
                                  });

    // Test with fringe spacing of 2.0
    double spacing = 2.0;

    auto frac = [](double x) { return x - std::floor(x); };
    auto fct = [spacing, frac](double x) { return std::abs(spacing * frac(x / spacing));};

    auto result = df::geo::fringes(insar, spacing);

    EXPECT_EQ(result.itemSize(), 1);
    EXPECT_EQ(result.count(), 4);

    EXPECT_NEAR(result.value(0), fct(0.0), 1e-10);
    EXPECT_NEAR(result.value(1), fct(2.5), 1e-10);
    EXPECT_NEAR(result.value(2), fct(5.0), 1e-10);
    EXPECT_NEAR(result.value(3), fct(7.5), 1e-10);
}

TEST(fringes, error_cases) {
    // Test with wrong itemSize
    df::GenSerie<double> wrong_size(2, {1.0, 0.0, 1.0, 0.0});
    EXPECT_THROW(df::geo::fringes(wrong_size, 1.0), std::runtime_error);

    // Test with negative spacing
    df::GenSerie<double> insar(1, {1.0, 2.0, 3.0});
    auto result =
        df::geo::fringes(insar, -2.0); // Should still work with abs values
    EXPECT_EQ(result.count(), 3);
}

TEST(insar, pipe_operator) {
    df::GenSerie<double> u(3, {1.0, 0.0, 0.0, 0.0, 1.0, 0.0});
    std::vector<double> los = {1.0, 0.0, 0.0};

    // Test insar with pipe operator
    auto result1 = u | df::geo::make_insar(los);
    EXPECT_EQ(result1.itemSize(), 1);
    EXPECT_EQ(result1.count(), 2);

    // Test chaining insar and fringes
    auto result2 = u | df::geo::make_insar(los) | df::geo::make_fringes(2.0);
    EXPECT_EQ(result2.itemSize(), 1);
    EXPECT_EQ(result2.count(), 2);
}

RUN_TESTS();