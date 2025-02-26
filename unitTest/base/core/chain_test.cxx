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

#include "../../TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/core/chain.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <string>

// Test for basic chain functionality with primitive types
TEST(Chain, BasicChaining) {
    MSG("Testing basic chaining with primitive types");

    // Integers
    df::Serie<int> ints1{1, 2, 3};
    df::Serie<int> ints2{4, 5, 6};
    df::Serie<int> ints3{7, 8, 9};

    // Chain two series
    auto result2 = df::chain(ints1, ints2);
    df::Serie<int> expected2{1, 2, 3, 4, 5, 6};
    COMPARE_SERIES(result2, expected2);

    // Chain three series
    auto result3 = df::chain(ints1, ints2, ints3);
    df::Serie<int> expected3{1, 2, 3, 4, 5, 6, 7, 8, 9};
    COMPARE_SERIES(result3, expected3);

    // Strings
    df::Serie<std::string> strings1{"a", "b", "c"};
    df::Serie<std::string> strings2{"d", "e", "f"};

    auto string_result = df::chain(strings1, strings2);
    df::Serie<std::string> expected_strings{"a", "b", "c", "d", "e", "f"};
    COMPARE_SERIES(string_result, expected_strings);
}

// Test for chaining with different sized series
TEST(Chain, DifferentSizes) {
    MSG("Testing chaining series of different sizes");

    df::Serie<double> s1{1.1, 2.2, 3.3};
    df::Serie<double> s2{4.4};
    df::Serie<double> s3{5.5, 6.6, 7.7, 8.8};

    auto result = df::chain(s1, s2, s3);
    df::Serie<double> expected{1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8};
    COMPARE_SERIES(result, expected);
}

// Test for chaining with empty series
TEST(Chain, EmptySeries) {
    MSG("Testing chaining with empty series");

    df::Serie<int> empty;
    df::Serie<int> ints{1, 2, 3};

    // Empty + Regular
    auto result1 = df::chain(empty, ints);
    COMPARE_SERIES(result1, ints);

    // Regular + Empty
    auto result2 = df::chain(ints, empty);
    COMPARE_SERIES(result2, ints);

    // Empty + Regular + Empty
    auto result3 = df::chain(empty, ints, empty);
    COMPARE_SERIES(result3, ints);

    // Empty + Empty
    auto result4 = df::chain(empty, empty);
    EXPECT_EQ(result4.size(), 0);
}

// Test for pipe functionality
TEST(Chain, PipeFunction) {
    MSG("Testing pipe functionality");

    df::Serie<int> s1{1, 2, 3};
    df::Serie<int> s2{4, 5, 6};
    df::Serie<int> s3{7, 8, 9};

    // Basic pipe usage
    auto result1 = s1 | df::bind_chain(s2);
    df::Serie<int> expected1{1, 2, 3, 4, 5, 6};
    COMPARE_SERIES(result1, expected1);

    // Multiple arguments
    auto result2 = s1 | df::bind_chain(s2, s3);
    df::Serie<int> expected2{1, 2, 3, 4, 5, 6, 7, 8, 9};
    COMPARE_SERIES(result2, expected2);

    // Combining with other operations
    auto result3 =
        s1 | df::bind_chain(s2) | df::bind_map([](int x) { return x * 2; });

    df::Serie<int> expected3{2, 4, 6, 8, 10, 12};
    COMPARE_SERIES(result3, expected3);
}

// Test with user-defined types
struct Point {
    double x, y;

    bool operator==(const Point &other) const {
        return x == other.x && y == other.y;
    }
};

std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "Point{" << p.x << ", " << p.y << "}";
    return os;
}

TEST(Chain, UserDefinedTypes) {
    MSG("Testing with user-defined types");

    df::Serie<Point> points1{{1.0, 2.0}, {3.0, 4.0}};

    df::Serie<Point> points2{{5.0, 6.0}, {7.0, 8.0}};

    auto result = df::chain(points1, points2);
    df::Serie<Point> expected{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}, {7.0, 8.0}};

    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i].x, expected[i].x);
        EXPECT_EQ(result[i].y, expected[i].y);
    }
}

// Test with Vector type
TEST(Chain, VectorType) {
    MSG("Testing with Vector type");

    df::Serie<Vector2> vectors1{{1.0, 0.0}, {0.0, 1.0}};

    df::Serie<Vector2> vectors2{{2.0, 0.0}, {0.0, 2.0}};

    auto result = df::chain(vectors1, vectors2);
    df::Serie<Vector2> expected{{1.0, 0.0}, {0.0, 1.0}, {2.0, 0.0}, {0.0, 2.0}};

    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_ARRAY_EQ(result[i], expected[i]);
    }
}

// Test performance with larger series
TEST(Chain, Performance) {
    MSG("Testing performance with larger series");

    const size_t SIZE = 10000;

    // Create test data
    std::vector<int> data1(SIZE);
    std::vector<int> data2(SIZE);

    for (size_t i = 0; i < SIZE; ++i) {
        data1[i] = static_cast<int>(i);
        data2[i] = static_cast<int>(i + SIZE);
    }

    df::Serie<int> serie1(data1);
    df::Serie<int> serie2(data2);

    // Time the chain operation
    double time_ms = TIMING([&]() {
        auto result = df::chain(serie1, serie2);
        EXPECT_EQ(result.size(), 2 * SIZE);
        EXPECT_EQ(result[0], 0);
        EXPECT_EQ(result[SIZE], SIZE);
        EXPECT_EQ(result[2 * SIZE - 1], 2 * SIZE - 1);
    });

    MSG("Chain operation on ", 2 * SIZE, " elements took ", time_ms, " ms");
}

// Main function to run all tests
RUN_TESTS()
