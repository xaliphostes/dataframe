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

#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/types.h>
#include <dataframe/utils/utils.h>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

void message(const String &msg) { std::cerr << msg << std::endl; }

void assertCondition(bool cond, const String &msg = "") {
    if (!cond) {
        if (msg.size() > 0) {
            std::cout << "Condition failed (msg = " << msg << ")!" << std::endl;
        } else {
            std::cout << "Condition failed!" << std::endl;
        }
        exit(-1);
    }
}

template <typename T> void assertEqual(const T &a, const T &b) {
    if (a != b) {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b
                  << std::endl;
        exit(-1);
    }
}

void assertDoubleEqual(double a, double b, double tol = 1e-7) {
    if (std::fabs(a - b) > tol) {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b
                  << std::endl;
        exit(-1);
    }
}

void assertArrayEqual(const Array &serie, const Array &array,
                      double tol = 1e-7) {
    if (serie.size() != array.size()) {
        std::cerr << "not same size: a=" << serie.size()
                  << ", b=" << array.size() << std::endl;
        exit(-1);
    }
    const auto &a = serie;
    for (uint32_t i = 0; i < a.size(); ++i) {
        if (std::fabs(a[i] - array[i]) > tol) {
            std::cerr << "not same values: " << a[i] << " " << array[i]
                      << ", diff=" << std::fabs(a[i] - array[i]) << std::endl;
            exit(-1);
        }
    }
}

void assertArrayEqual(const Strings &a, const Strings &b) {
    if (a.size() != b.size()) {
        std::cerr << "not same size: a=" << a.size() << ", b=" << b.size()
                  << std::endl;
        exit(-1);
    }

    for (uint32_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            std::cerr << "not same values: " << a[i] << " " << b[i]
                      << std::endl;
            exit(-1);
        }
    }
}

void assertSerieEqual(const df::Serie &s1, const Array &s2, double tol = 1e-7) {
    assertArrayEqual(s1.asArray(), s2, tol);
}

void assertSerieEqual(const df::Serie &s1, const df::Serie &s2,
                      double tol = 1e-7) {
    assertEqual<int>(s1.itemSize(), s2.itemSize());
    assertEqual<int>(s1.dimension(), s2.dimension());
    assertEqual<uint32_t>(s1.count(), s2.count());
    assertEqual<uint32_t>(s1.size(), s2.size()); // we never know ;-)
    assertArrayEqual(s1.asArray(), s2.asArray(), tol);
}

template <typename CB> void shouldThrowError(CB &&cb) {
    try {
        cb();
        std::cerr << "not throwing an error!\n";
        exit(-1);
    } catch (std::invalid_argument &e) {
        // ok
        df::error(e.what());
    }
}

template <typename CB> void shouldNotThrowError(CB &&cb) {
    try {
        cb();
    } catch (std::invalid_argument &e) {
        df::error(e.what());
        exit(-1);
    }
}

// -----------------------------------------------------------------------------------------------
//                          A la Google test framework: gtest using cmake
// -----------------------------------------------------------------------------------------------

namespace test {

using TestFunction = std::function<void()>;
struct TestInfo {
    const char *name;
    const char *fixture;
    TestFunction fn;
};
inline std::vector<TestInfo> tests;

inline void register_test(const char *name, const char *fixture,
                          TestFunction fn) {
    tests.emplace_back(TestInfo{name, fixture, fn});
}

} // namespace test

#define TEST(name, fixture)                                                    \
    void name##_##fixture();                                                   \
    static struct register_##name##_##fixture {                                \
        register_##name##_##fixture() {                                        \
            test::register_test(#name, #fixture, name##_##fixture);            \
        }                                                                      \
    } register_##name##_##fixture##_instance;                                  \
    void name##_##fixture()

#define RUN_TESTS()                                                            \
    int main() {                                                               \
        for (const auto &test : test::tests) {                                 \
            std::cout << "***************************************************" \
                         "*******"                                             \
                      << std::endl;                                            \
            std::cout << "Running test [" << test.name << "/" << test.fixture  \
                      << "]" << std::endl;                                     \
            std::cout << "***************************************************" \
                         "*******"                                             \
                      << std::endl;                                            \
            test.fn();                                                         \
        }                                                                      \
        return 0;                                                              \
    }

#define EXPECT_EQ(a, b) assertEqual(a, b)
#define EXPECT_DOUBLE_EQ(a, b) assertDoubleEqual(a, b)
#define EXPECT_STREQ(a, b) assertEqual<String>(a, b)
#define EXPECT_TRUE(cond) assertCondition(cond)
#define EXPECT_FALSE(cond) assertCondition(!(cond))
#define EXPECT_THROW(stmt) shouldThrowError([&]() { stmt; })
#define EXPECT_NO_THROW(stmt) shouldNotThrowError([&]() { stmt; })
// -----------------------------------------------------------------------------------------------
