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
#include <dataframe/functional/forEach.h>
#include <dataframe/functional/map.h>

TEST(map, scalar_scalar) {
    df::GenSerie<int> s1(1, {1, 2, 3, 4});

    // Scalar → Scalar
    auto doubled = df::map([](int v, uint32_t) { return v * 2; }, s1);

    EXPECT_ARRAY_EQ(doubled.asArray(),
                    df::GenSerie<int>(1, {2, 4, 6, 8}).asArray());
}

TEST(map, scalar_vector) {
    df::GenSerie<float> s1(1, {1, 2, 3, 4});
    // Scalar → Vector
    auto expanded = df::map(
        [](float v, uint32_t) {
            return Array<float>{v, v * v, v * v * v}; // Retourne un Array
        },
        s1);

    EXPECT_ARRAY_EQ(
        expanded.asArray(),
        df::GenSerie<float>(1, {1, 1, 1, 2, 4, 8, 3, 9, 27, 4, 16, 64})
            .asArray());
}

TEST(map, makeMap_1) {
    df::GenSerie<float> s1(1, {1, 2, 3, 4});

    auto makeDouble = df::make_map(
        [](float v, uint32_t) { return Array<float>{v, v * v, v * v * v}; });
    auto result = makeDouble(s1);
    EXPECT_ARRAY_EQ(result.asArray(),
                    df::GenSerie<float>(1, {1.0000, 1.0000, 1.0000, 2.0000,
                                            4.0000, 8.0000, 3.0000, 9.0000,
                                            27.0000, 4.0000, 16.0000, 64.0000})
                        .asArray());
}

TEST(map, makeMap_2) {
    // Create a map operation that squares each value
    auto square = df::make_map([](double x, uint32_t) { return x * x; });

    // Use it on different series
    df::GenSerie<double> s1(1, {1, 2, 3, 4});
    auto squared = square(s1); // Returns a new series with squared values
    EXPECT_ARRAY_EQ(squared.asArray(),
                    df::GenSerie<double>(1, {1, 4, 9, 16}).asArray());

    // Create a map operation that combines two series
    auto add = df::make_map([](double x, double y, uint32_t) { return x + y; });
    df::GenSerie<double> s2(1, {5, 6, 7, 8});
    auto sum = add(s1, s2); // Returns a new series with sum of elements
    EXPECT_ARRAY_EQ(sum.asArray(),
                    df::GenSerie<double>(1, {6, 8, 10, 12}).asArray());
}

TEST(map, makeMap_3) {
    df::GenSerie<double> points(3, {1, 2, 3, 4, 5, 6});

    MSG("Scalar operation on vector serie");
    {
        // Create a map operation that scales each component by 2
        auto scale2 = df::make_map([](double x, uint32_t) { return x * 2; });
        auto scaled = scale2(points);

        CHECK(scaled.itemSize() == 3);
        CHECK(scaled.count() == 2);

        // First point should be [2,4,6]
        auto p1 = scaled.array(0);
        CHECK(p1[0] == 2);
        CHECK(p1[1] == 4);
        CHECK(p1[2] == 6);

        // Second point should be [8,10,12]
        auto p2 = scaled.array(1);
        CHECK(p2[0] == 8);
        CHECK(p2[1] == 10);
        CHECK(p2[2] == 12);
    }

    MSG("Vector to vector operation");
    {
        // Create a map operation that computes the squared norm of each point and returns [x²,y²,z²]
        auto squared_components = df::make_map([](const std::vector<double>& v, uint32_t) {
            return std::vector<double>{v[0]*v[0], v[1]*v[1], v[2]*v[2]};
        });
        
        auto result = squared_components(points);

        CHECK(result.itemSize() == 3);
        CHECK(result.count() == 2);

        // First point [1,2,3] -> [1,4,9]
        auto p1 = result.array(0);
        CHECK(p1[0] == 1);
        CHECK(p1[1] == 4);
        CHECK(p1[2] == 9);

        // Second point [4,5,6] -> [16,25,36]
        auto p2 = result.array(1);
        CHECK(p2[0] == 16);
        CHECK(p2[1] == 25);
        CHECK(p2[2] == 36);
    }

    MSG("Vector to scalar operation")
    {
        // Create a map operation that computes the magnitude of each 3D point
        auto magnitude = df::make_map([](const std::vector<double>& v, uint32_t) {
            return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        });
        
        auto result = magnitude(points);

        CHECK(result.itemSize() == 1);
        CHECK(result.count() == 2);

        // First point [1,2,3] -> sqrt(14)
        EXPECT_NEAR(result.value(0), std::sqrt(14), 1e-10);

        // Second point [4,5,6] -> sqrt(77)
        EXPECT_NEAR(result.value(1), std::sqrt(77), 1e-10);
    }
}

RUN_TESTS()