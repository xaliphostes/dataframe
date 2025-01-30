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
#include "TEST.h"
#include <array>
#include <complex>
#include <dataframe/utils/zip.h>
#include <dataframe/utils/unzip.h>
#include <string>
#include <vector>

using namespace df;

TEST(unzip, zip_unzip_roundtrip) {
    MSG("Testing zip followed by unzip");

    // Original series
    Serie<int> s1{1, 2, 3};
    Serie<double> s2{1.1, 2.2, 3.3};
    Serie<std::string> s3{"a", "b", "c"};

    // Zip them
    auto zipped = zip(s1, s2, s3);

    // Unzip back
    auto [ints, doubles, strings] = unzip(zipped);

    // Check that we got back our original data
    EXPECT_EQ(ints.size(), 3);
    EXPECT_EQ(doubles.size(), 3);
    EXPECT_EQ(strings.size(), 3);

    for (size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(ints[i], s1[i]);
        EXPECT_NEAR(doubles[i], s2[i], 1e-10);
        EXPECT_EQ(strings[i], s3[i]);
    }
}

TEST(unzip, basic) {
    MSG("Testing basic unzip operations");

    // Create a Serie of tuples
    Serie<std::tuple<int, double>> s1{
        std::make_tuple(1, 1.1),
        std::make_tuple(2, 2.2),
        std::make_tuple(3, 3.3)
    };

    // Unzip into separate series
    auto [ints, doubles] = unzip(s1);

    // Check first Serie
    EXPECT_EQ(ints.size(), 3);
    EXPECT_EQ(ints[0], 1);
    EXPECT_EQ(ints[1], 2);
    EXPECT_EQ(ints[2], 3);

    // Check second Serie
    EXPECT_EQ(doubles.size(), 3);
    EXPECT_NEAR(doubles[0], 1.1, 1e-10);
    EXPECT_NEAR(doubles[1], 2.2, 1e-10);
    EXPECT_NEAR(doubles[2], 3.3, 1e-10);
}

TEST(unzip, complex_types) {
    MSG("Testing unzip with complex types");

    using Vector2D = std::array<double, 2>;
    
    // Create a Serie of tuples with complex types
    Serie<std::tuple<std::complex<double>, Vector2D>> s1{
        std::make_tuple(std::complex<double>(1.0, 1.0), Vector2D{1.0, 2.0}),
        std::make_tuple(std::complex<double>(2.0, 2.0), Vector2D{3.0, 4.0})
    };

    auto [complexes, vectors] = unzip(s1);

    // Check complex numbers
    EXPECT_EQ(complexes.size(), 2);
    EXPECT_NEAR(complexes[0].real(), 1.0, 1e-10);
    EXPECT_NEAR(complexes[0].imag(), 1.0, 1e-10);

    // Check vectors
    EXPECT_EQ(vectors.size(), 2);
    EXPECT_NEAR(vectors[0][0], 1.0, 1e-10);
    EXPECT_NEAR(vectors[0][1], 2.0, 1e-10);
}

TEST(unzip, triple) {
    MSG("Testing triple unzip");

    Serie<std::tuple<int, double, std::string>> s1{
        std::make_tuple(1, 1.1, "a"),
        std::make_tuple(2, 2.2, "b"),
        std::make_tuple(3, 3.3, "c")
    };

    auto [ints, doubles, strings] = unzip(s1);

    EXPECT_EQ(ints.size(), 3);
    EXPECT_EQ(doubles.size(), 3);
    EXPECT_EQ(strings.size(), 3);

    EXPECT_EQ(ints[0], 1);
    EXPECT_NEAR(doubles[0], 1.1, 1e-10);
    EXPECT_EQ(strings[0], "a");
}

TEST(unzip, with_custom_types) {
    MSG("Testing unzip with custom types");

    struct Point {
        double x, y;
        Point(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    Serie<std::tuple<Point, double>> s1{
        std::make_tuple(Point(1,1), 0.5),
        std::make_tuple(Point(2,2), 1.0),
        std::make_tuple(Point(3,3), 1.5)
    };

    auto [points, weights] = unzip(s1);

    EXPECT_EQ(points.size(), 3);
    EXPECT_EQ(weights.size(), 3);
    EXPECT_EQ(points[0].x, 1); EXPECT_EQ(points[0].y, 1);
    EXPECT_NEAR(weights[0], 0.5, 1e-10);
}

RUN_TESTS()