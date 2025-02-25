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

#include "../TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/core/forEach.h>
#include <dataframe/core/map.h>
#include <dataframe/core/reduce.h>

// ------------------------------------------------

struct MyData {
    MyData(int a, double b, std::string c) : a(a), b(b), c(c) {}
    int a;
    double b;
    std::string c;
};
std::ostream &operator<<(std::ostream &o, const MyData &s) {
    o << "MyData(" << s.a << ", " << s.b << ", " << s.c << ")";
    return o;
}

// ------------------------------------------------

TEST(Serie, basic) {
    df::Serie<int> scalars{1, 2, 3, 4, 5};
    df::print(scalars);
}

TEST(Serie, user) {
    df::Serie<MyData> serie({MyData(1, 2.0, "test"), MyData(3, 4.0, "test2")});
    df::print(serie);
}

TEST(Serie, matrices) {
    df::Serie<Stress3D> stress{
        {1, 2, 3, 4, 5, 6},       {7, 8, 9, 10, 11, 12},
        {13, 14, 15, 16, 17, 18}, {19, 20, 21, 22, 23, 24},
        {25, 26, 27, 28, 29, 30}, {31, 32, 33, 34, 35, 36},
    };
    df::print(stress);

    // ------------------------------------------------

    df::Serie<Stress3D> stress2(10, {1, 0, 0, 1, 0, 1});
    df::forEach(
        [](const Stress3D &s, size_t) {
            EXPECT_ARRAY_EQ(s, Stress3D({1, 0, 0, 1, 0, 1}));
        },
        stress2);
}

TEST(serie, size_constructor) {
    // Test default value initialization
    df::Serie<int> s1(5);
    EXPECT_TRUE(s1.size() == 5);
    for (size_t i = 0; i < s1.size(); ++i) {
        EXPECT_TRUE(s1[i] == 0); // int default is 0
    }

    // Test with specific value
    df::Serie<double> s2(3, 1.5);
    EXPECT_TRUE(s2.size() == 3);
    for (size_t i = 0; i < s2.size(); ++i) {
        EXPECT_TRUE(s2[i] == 1.5);
    }

    // Test with string
    df::Serie<std::string> s3(2, "test");
    EXPECT_TRUE(s3.size() == 2);
    for (size_t i = 0; i < s3.size(); ++i) {
        EXPECT_TRUE(s3[i] == "test");
    }

    // Test empty size
    df::Serie<int> s4(0);
    EXPECT_TRUE(s4.size() == 0);
    EXPECT_TRUE(s4.empty());
}

TEST(serie, size_constructor_edge_cases) {
    // Very large size
    df::Serie<char> s1(1000000);
    EXPECT_TRUE(s1.size() == 1000000);

    // Size constructor with custom type
    struct CustomType {
        int x = 42;
    };
    df::Serie<CustomType> s2(3);
    EXPECT_TRUE(s2.size() == 3);
    for (size_t i = 0; i < s2.size(); ++i) {
        EXPECT_TRUE(s2[i].x == 42);
    }
}

RUN_TESTS();