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

#include "../../../TEST.h"
#include <cmath>
#include <dataframe/math/nan/interpolation.h>
#include <dataframe/math/nan/set.h>

TEST(NanOperations, SetNanAtIndices) {
    auto serie = df::Serie<double>({1.0, 2.0, 3.0, 4.0, 5.0});
    auto result = df::nan::set_at(serie, {1, 3});

    EXPECT_TRUE(std::isnan(result[1]));
    EXPECT_TRUE(std::isnan(result[3]));
    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[2], 3.0);
    EXPECT_EQ(result[4], 5.0);
}

TEST(NanOperations, SetNanWhere) {
    auto serie = df::Serie<double>({1.0, 2.0, 3.0, 4.0, 5.0});

    // Set NaN for values greater than 3
    auto result =
        df::nan::set_where(serie, [](double val, size_t) { return val > 3.0; });

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 2.0);
    EXPECT_EQ(result[2], 3.0);
    EXPECT_TRUE(std::isnan(result[3]));
    EXPECT_TRUE(std::isnan(result[4]));
}

TEST(NanOperations, FindNan) {
    auto serie = df::Serie<double>({1.0, std::nan(""), 3.0, std::nan(""), 5.0});
    auto indices = df::nan::find(serie);

    EXPECT_EQ(indices.size(), 2);
    EXPECT_EQ(indices[0], 1);
    EXPECT_EQ(indices[1], 3);
}

TEST(NanOperations, CountNan) {
    auto serie = df::Serie<double>({1.0, std::nan(""), 3.0, std::nan(""), 5.0});
    auto c = df::nan::count(serie);
    EXPECT_EQ(c, 2);
}

TEST(NanOperations, HasNan) {
    auto serie1 = df::Serie<double>({1.0, 2.0, 3.0});
    auto serie2 = df::Serie<double>({1.0, std::nan(""), 3.0});

    EXPECT_FALSE(df::nan::has(serie1));
    EXPECT_TRUE(df::nan::has(serie2));
}

TEST(Interpolation, LinearMethod) {
    auto serie = df::Serie<double>({1.0, std::nan(""), std::nan(""), 4.0});
    auto result = df::nan::interpolate(serie, df::nan::FillMethod::LINEAR);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_NEAR(result[1], 2.0, 1e-10);
    EXPECT_NEAR(result[2], 3.0, 1e-10);
    EXPECT_EQ(result[3], 4.0);
}

TEST(Interpolation, NearestMethod) {
    auto serie = df::Serie<double>({1.0, std::nan(""), std::nan(""), 4.0});
    auto result = df::nan::interpolate(serie, df::nan::FillMethod::NEAREST);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 1.0);
    EXPECT_EQ(result[2], 4.0);
    EXPECT_EQ(result[3], 4.0);
}

TEST(Interpolation, PreviousMethod) {
    auto serie = df::Serie<double>({1.0, std::nan(""), std::nan(""), 4.0});
    auto result = df::nan::interpolate(serie, df::nan::FillMethod::PREVIOUS);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 1.0);
    EXPECT_EQ(result[2], 1.0);
    EXPECT_EQ(result[3], 4.0);
}

TEST(Interpolation, NextMethod) {
    auto serie = df::Serie<double>({1.0, std::nan(""), std::nan(""), 4.0});
    auto result = df::nan::interpolate(serie, df::nan::FillMethod::NEXT);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 4.0);
    EXPECT_EQ(result[2], 4.0);
    EXPECT_EQ(result[3], 4.0);
}

TEST(Interpolation, MeanMethod) {
    auto serie = df::Serie<double>({1.0, std::nan(""), std::nan(""), 5.0});
    auto result = df::nan::interpolate(serie, df::nan::FillMethod::MEAN);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 3.0);
    EXPECT_EQ(result[2], 3.0);
    EXPECT_EQ(result[3], 5.0);
}

TEST(Interpolation, EdgeCases) {
    // Test empty serie
    auto empty = df::Serie<double>({});
    auto result1 = df::nan::interpolate(empty, df::nan::FillMethod::LINEAR);
    EXPECT_EQ(result1.size(), 0);

    // Test serie with all NaN
    auto all_nan =
        df::Serie<double>({std::nan(""), std::nan(""), std::nan("")});
    auto result2 = df::nan::interpolate(all_nan, df::nan::FillMethod::LINEAR);
    EXPECT_EQ(result2.size(), 3);
    for (size_t i = 0; i < result2.size(); ++i) {
        EXPECT_EQ(result2[i], 0.0); // Default value for all NaN case
    }

    // Test serie with NaN at edges
    auto edge_nan = df::Serie<double>({std::nan(""), 2.0, 3.0, std::nan("")});
    auto result3 = df::nan::interpolate(edge_nan, df::nan::FillMethod::LINEAR);
    EXPECT_EQ(result3[0], 2.0); // First value gets filled with nearest
    EXPECT_EQ(result3[1], 2.0);
    EXPECT_EQ(result3[2], 3.0);
    EXPECT_EQ(result3[3], 3.0); // Last value gets filled with nearest
}

TEST(Interpolation, MixedOperations) {
    // Test combination of set_nan and interpolate
    auto serie = df::Serie<double>({1.0, 2.0, 3.0, 4.0, 5.0});

    // Set values > 3 to NaN
    auto with_nan =
        df::nan::set_where(serie, [](double val, size_t) { return val > 3.0; });

    df::print(serie);
    df::print(with_nan);

    // Interpolate the NaN values
    auto result = df::nan::interpolate(with_nan, df::nan::FillMethod::LINEAR);
    df::print(result);

    EXPECT_EQ(result[0], 1.0);
    EXPECT_EQ(result[1], 2.0);
    EXPECT_EQ(result[2], 3.0);
    EXPECT_NEAR(result[3], 3.5, 1e-10);
    EXPECT_EQ(result[4], 4.0);
}

RUN_TESTS();