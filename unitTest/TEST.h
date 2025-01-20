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
#include <dataframe/Serie.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/print.h>
#include <dataframe/types.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------------------------
//                     A la Google test framework: that is to say, GEST using
//                     CMAKE
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

#define EXPECT_EQ(val1, val2)                                                  \
    {                                                                          \
        auto v1 = (val1);                                                      \
        auto v2 = (val2);                                                      \
        if (v1 != v2) {                                                        \
            std::stringstream ss;                                              \
            ss << "Expected " << v1 << " to equal " << v2 << " but |" << v1    \
               << " != " << v2 << "|";                                         \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

#define EXPECT_NEAR(val1, val2, tol)                                           \
    {                                                                          \
        auto v1 = (val1);                                                      \
        auto v2 = (val2);                                                      \
        if (std::abs(v1 - v2) > tol) {                                         \
            std::stringstream ss;                                              \
            ss << "Expected " << v1 << " to be near " << v2                    \
               << " (tolerance=" << tol << ")"                                 \
               << " but |" << v1 << " - " << v2 << "| = " << std::abs(v1 - v2) \
               << " > " << tol;                                                \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

#define EXPECT_THROW(statement, exception_type)                                \
    {                                                                          \
        bool caught_expected = false;                                          \
        try {                                                                  \
            statement;                                                         \
        } catch (const exception_type &) {                                     \
            caught_expected = true;                                            \
        } catch (const std::exception &e) {                                    \
            std::stringstream ss;                                              \
            ss << "Expected " << #statement << " to throw " << #exception_type \
               << " but it threw a different exception: " << e.what();         \
            throw std::runtime_error(ss.str());                                \
        } catch (...) {                                                        \
            std::stringstream ss;                                              \
            ss << "Expected " << #statement << " to throw " << #exception_type \
               << " but it threw an unknown exception";                        \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
        if (!caught_expected) {                                                \
            std::stringstream ss;                                              \
            ss << "Expected " << #statement << " to throw " << #exception_type \
               << " but it didn't throw";                                      \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

// String comparison
#define EXPECT_STREQ(str1, str2)                                               \
    {                                                                          \
        std::string s1 = (str1);                                               \
        std::string s2 = (str2);                                               \
        if (s1 != s2) {                                                        \
            std::stringstream ss;                                              \
            ss << "Expected strings to be equal\n"                             \
               << "    Got     : '" << s1 << "'\n"                             \
               << "    Expected: '" << s2 << "'";                              \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

// Boolean condition true
#define EXPECT_TRUE(condition)                                                 \
    {                                                                          \
        if (!(condition)) {                                                    \
            std::stringstream ss;                                              \
            ss << "Expected " << #condition << " to be true";                  \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

// Boolean condition false
#define EXPECT_FALSE(condition)                                                \
    {                                                                          \
        if (condition) {                                                       \
            std::stringstream ss;                                              \
            ss << "Expected " << #condition << " to be false";                 \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

// No exception thrown
#define EXPECT_NO_THROW(statement)                                             \
    {                                                                          \
        try {                                                                  \
            statement;                                                         \
        } catch (const std::exception &e) {                                    \
            std::stringstream ss;                                              \
            ss << "Expected " << #statement << " not to throw, but it threw "  \
               << "exception: " << e.what();                                   \
            throw std::runtime_error(ss.str());                                \
        } catch (...) {                                                        \
            std::stringstream ss;                                              \
            ss << "Expected " << #statement << " not to throw, but it threw "  \
               << "an unknown exception";                                      \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
    }

#define EXPECT_ARRAY_NEAR(arr1, arr2, tol)                                     \
    {                                                                          \
        auto a1 = (arr1);                                                      \
        auto a2 = (arr2);                                                      \
        if (a1.size() != a2.size()) {                                          \
            std::stringstream ss;                                              \
            ss << "Array sizes differ: " << a1.size() << " != " << a2.size();  \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
        for (size_t i = 0; i < a1.size(); ++i) {                               \
            if (std::abs(a1[i] - a2[i]) > tol) {                               \
                std::stringstream ss;                                          \
                ss << "Arrays differ at index " << i << ": " << a1[i]          \
                   << " != " << a2[i]                                          \
                   << " (diff = " << std::abs(a1[i] - a2[i])                   \
                   << ", tolerance = " << tol << ")";                          \
                throw std::runtime_error(ss.str());                            \
            }                                                                  \
        }                                                                      \
    }

#define EXPECT_ARRAY_EQ(arr1, arr2)                                            \
    {                                                                          \
        auto a1 = (arr1);                                                      \
        auto a2 = (arr2);                                                      \
        if (a1.size() != a2.size()) {                                          \
            std::stringstream ss;                                              \
            ss << "Array sizes differ: " << a1.size() << " != " << a2.size();  \
            throw std::runtime_error(ss.str());                                \
        }                                                                      \
        for (size_t i = 0; i < a1.size(); ++i) {                               \
            if (a1[i] != a2[i]) {                                              \
                std::stringstream ss;                                          \
                ss << "Arrays differ at index " << i << ": " << a1[i]          \
                   << " != " << a2[i]                                          \
                   << " (diff = " << std::abs(a1[i] - a2[i]) << ")";           \
                throw std::runtime_error(ss.str());                            \
            }                                                                  \
        }                                                                      \
    }

template <typename T> struct ParsedSerie {
    std::string type;
    uint32_t itemSize;
    uint32_t count;
    uint32_t dimension;
    std::vector<T> values;
};

#define EXPECT_SERIE_EQ(serie, expected)                                       \
    {                                                                          \
        EXPECT_EQ(serie.count(), expected.count);                              \
        EXPECT_EQ(serie.itemSize(), expected.itemSize);                        \
        EXPECT_EQ(serie.dimension(), expected.dimension);                      \
        EXPECT_STREQ(serie.type(), expected.type);                             \
        EXPECT_ARRAY_EQ(serie.asArray(), expected.values);                     \
    }

class TestException : public std::runtime_error {
  public:
    TestException(const std::string &message) : std::runtime_error(message) {}
};

#define CHECK(condition)                                                       \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::stringstream ss;                                              \
            ss << "Check failed: " << #condition << "\n";                      \
            ss << "File: " << __FILE__ << "\n";                                \
            ss << "Line: " << __LINE__ << "\n";                                \
            throw TestException(ss.str());                                     \
        }                                                                      \
    } while (0)

#define MSG(msg) std::cout << "---> " << msg << std::endl;




// ====================================================================

// Using TEST_CASE and SUB_CASE


// // Helper class for approximate floating point comparisons
// class Approx {
//   public:
//     explicit Approx(double value, double epsilon = 1e-6)
//         : value_(value), epsilon_(epsilon) {}

//     bool compare(double other) const {
//         return std::abs(value_ - other) <= epsilon_;
//     }

//     friend bool operator==(double lhs, const Approx &rhs) {
//         return rhs.compare(lhs);
//     }

//     friend bool operator==(const Approx &lhs, double rhs) {
//         return lhs.compare(rhs);
//     }

//   private:
//     double value_;
//     double epsilon_;
// };

// Main CHECK macro


// Test case macro
// #define TEST_CASE(name) \
//     void test_case_##name(); \
//     struct TestRunner##name { \
//         TestRunner##name() { \
//             std::cout << "Running test case: " << #name << std::endl; \
//             try { \
//                 test_case_##name(); \
//                 std::cout << "Test passed: " << #name << std::endl; \
//             } catch (const TestException &e) { \
//                 std::cerr << "Test failed: " << #name << "\n" \
//                           << e.what() << std::endl; \
//                 throw; \
//             } \
//         } \
//     } test_runner_##name; \ void test_case_##name()

// // Subcase macro
// #define SUBCASE(name) \
//     std::cout << "  Subcase: " << name << std::endl; \ if (true)