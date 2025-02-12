/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include "../../TEST.h"
#include <array>
#include <complex>
#include <dataframe/meta.h>
#include <dataframe/utils/zip.h>
#include <string>
#include <vector>

using namespace df;

TEST(zip, basic) {
    MSG("Testing basic zip operations");

    // Test integer and double zip
    Serie<int> s1{1, 2, 3};
    Serie<double> s2{1.1, 2.2, 3.3};
    auto result = zip(s1, s2);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(std::get<0>(result[0]), 1);
    EXPECT_NEAR(std::get<1>(result[0]), 1.1, 1e-10);
    EXPECT_EQ(std::get<0>(result[1]), 2);
    EXPECT_NEAR(std::get<1>(result[1]), 2.2, 1e-10);

    // Test with strings
    Serie<std::string> s3{"a", "b", "c"};
    auto result_str = zip(s1, s3);
    EXPECT_EQ(result_str.size(), 3);
    EXPECT_EQ(std::get<0>(result_str[0]), 1);
    EXPECT_EQ(std::get<1>(result_str[0]), "a");

    // Test error on size mismatch
    Serie<int> s4{1, 2};
    EXPECT_THROW(zip(s1, s4), std::runtime_error);
}

TEST(zip, complex_types) {
    MSG("Testing zip with complex types");

    // Test with complex numbers
    Serie<std::complex<double>> c1{{1.0, 1.0}, {2.0, 2.0}};
    Serie<std::complex<double>> c2{{0.5, 0.5}, {1.5, 1.5}};
    auto complex_result = zip(c1, c2);

    EXPECT_EQ(complex_result.size(), 2);
    EXPECT_NEAR(std::get<0>(complex_result[0]).real(), 1.0, 1e-10);
    EXPECT_NEAR(std::get<1>(complex_result[0]).real(), 0.5, 1e-10);

    // Test with arrays
    using Vector2D = std::array<double, 2>;
    Serie<Vector2D> v1{Vector2D{1.0, 2.0}, Vector2D{3.0, 4.0}};
    Serie<Vector2D> v2{Vector2D{0.1, 0.2}, Vector2D{0.3, 0.4}};
    auto vector_result = zip(v1, v2);

    EXPECT_EQ(vector_result.size(), 2);
    EXPECT_NEAR(std::get<0>(vector_result[0])[0], 1.0, 1e-10);
    EXPECT_NEAR(std::get<1>(vector_result[0])[0], 0.1, 1e-10);
}

TEST(zip, triple) {
    MSG("Testing triple zip");

    Serie<int> s1{1, 2, 3};
    Serie<double> s2{1.1, 2.2, 3.3};
    Serie<std::string> s3{"a", "b", "c"};

    auto result = zip(s1, s2, s3);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(std::get<0>(result[0]), 1);
    EXPECT_NEAR(std::get<1>(result[0]), 1.1, 1e-10);
    EXPECT_EQ(std::get<2>(result[0]), "a");
}

TEST(zip, with_operations) {
    MSG("Testing zip with operations");

    Serie<int> values{1, 2, 3};
    Serie<double> weights{0.5, 1.0, 1.5};

    // Calculate weighted values
    auto weighted = zip(values, weights).map([](const auto &pair, size_t) {
        auto [value, weight] = pair;
        return value * weight;
    });

    EXPECT_EQ(weighted.size(), 3);
    EXPECT_NEAR(weighted[0], 0.5, 1e-10);
    EXPECT_NEAR(weighted[1], 2.0, 1e-10);
    EXPECT_NEAR(weighted[2], 4.5, 1e-10);
}

TEST(zip, custom_types) {
    MSG("Testing zip with custom types");

    struct Point {
        double x, y;
        Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
    };

    Serie<Point> points{Point(1, 1), Point(2, 2), Point(3, 3)};
    Serie<double> weights{0.5, 1.0, 1.5};

    auto weighted_points = zip(points, weights).map([](const auto &pair, size_t) {
        auto [point, weight] = pair;
        return Point(point.x * weight, point.y * weight);
    });

    EXPECT_EQ(weighted_points.size(), 3);
    EXPECT_NEAR(weighted_points[0].x, 0.5, 1e-10);
    EXPECT_NEAR(weighted_points[1].x, 2.0, 1e-10);
    EXPECT_NEAR(weighted_points[2].x, 4.5, 1e-10);
}

RUN_TESTS()
