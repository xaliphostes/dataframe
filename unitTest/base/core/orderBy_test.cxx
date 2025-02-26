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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/map.h>
#include <dataframe/core/orderBy.h>
#include <dataframe/core/pipe.h>
#include <string>

// Define a simple struct for testing complex types
struct Person {
    std::string name;
    int age;
    double salary;

    // Define equality operator for testing
    bool operator==(const Person &other) const {
        return name == other.name && age == other.age &&
               std::abs(salary - other.salary) < 1e-6;
    }
};

// Output operator for Person, useful for test failure messages
std::ostream &operator<<(std::ostream &os, const Person &p) {
    os << "Person{name=\"" << p.name << "\", age=" << p.age
       << ", salary=" << p.salary << "}";
    return os;
}

// Basic test for orderBy with simple numeric types
TEST(OrderBy, BasicNumericSorting) {
    MSG("Testing basic numeric sorting");

    // Create a test serie
    df::Serie<double> values{5.0, 2.0, 8.0, 1.0, 9.0};

    // Sort in ascending order (default)
    auto ascending = df::orderBy(values, [](double x) { return x; });
    df::Serie<double> expected_asc{1.0, 2.0, 5.0, 8.0, 9.0};
    COMPARE_SERIES(ascending, expected_asc);

    // Sort in descending order
    auto descending = df::orderBy(values, [](double x) { return x; }, false);
    df::Serie<double> expected_desc{9.0, 8.0, 5.0, 2.0, 1.0};
    COMPARE_SERIES(descending, expected_desc);
}

// Test for orderBy with a custom key function
TEST(OrderBy, CustomKeyFunction) {
    MSG("Testing sorting with custom key function");

    // Create a test serie
    df::Serie<double> values{-5.0, 3.0, -10.0, 7.0, 2.0};

    // Sort by absolute value
    auto by_abs = df::orderBy(values, [](double x) { return std::abs(x); });
    df::Serie<double> expected{2.0, 3.0, -5.0, 7.0, -10.0};
    COMPARE_SERIES(by_abs, expected);

    // Sort by square value
    auto by_square = df::orderBy(values, [](double x) { return x * x; });
    df::Serie<double> expected_sq{2.0, 3.0, -5.0, 7.0, -10.0};
    COMPARE_SERIES(by_square, expected_sq);
}

// Test for orderBy with complex object types
TEST(OrderBy, ComplexObjects) {
    MSG("Testing sorting complex objects");

    // Create test data
    df::Serie<Person> people{{"Alice", 30, 75000.0},
                             {"Bob", 25, 65000.0},
                             {"Charlie", 40, 90000.0},
                             {"David", 35, 85000.0},
                             {"Eve", 28, 70000.0}};

    // Sort by age
    auto by_age = df::orderBy(people, [](const Person &p) { return p.age; });
    df::Serie<Person> expected_age{{"Bob", 25, 65000.0},
                                   {"Eve", 28, 70000.0},
                                   {"Alice", 30, 75000.0},
                                   {"David", 35, 85000.0},
                                   {"Charlie", 40, 90000.0}};
    COMPARE_SERIES(by_age, expected_age);

    // Sort by name
    auto by_name = df::orderBy(people, [](const Person &p) { return p.name; });
    df::Serie<Person> expected_name{{"Alice", 30, 75000.0},
                                    {"Bob", 25, 65000.0},
                                    {"Charlie", 40, 90000.0},
                                    {"David", 35, 85000.0},
                                    {"Eve", 28, 70000.0}};
    COMPARE_SERIES(by_name, expected_name);

    // Sort by salary (descending)
    auto by_salary =
        df::orderBy(people, [](const Person &p) { return p.salary; }, false);
    df::Serie<Person> expected_salary{{"Charlie", 40, 90000.0},
                                      {"David", 35, 85000.0},
                                      {"Alice", 30, 75000.0},
                                      {"Eve", 28, 70000.0},
                                      {"Bob", 25, 65000.0}};
    COMPARE_SERIES(by_salary, expected_salary);
}

// Test for edge cases
TEST(OrderBy, EdgeCases) {
    MSG("Testing edge cases");

    // Empty series
    df::Serie<int> empty;
    auto sorted_empty = df::orderBy(empty, [](int x) { return x; });
    EXPECT_EQ(sorted_empty.size(), 0);

    // Single element series
    df::Serie<int> single{42};
    auto sorted_single = df::orderBy(single, [](int x) { return x; });
    EXPECT_EQ(sorted_single.size(), 1);
    EXPECT_EQ(sorted_single[0], 42);

    // Series with duplicate keys
    df::Serie<Person> duplicates{
        {"Alice", 30, 75000.0}, {"Bob", 30, 65000.0}, {"Charlie", 30, 90000.0}};
    auto sorted_dups =
        df::orderBy(duplicates, [](const Person &p) { return p.age; });
    EXPECT_EQ(sorted_dups.size(), 3);
    // Original order of equal keys should be preserved (stable sort)
    EXPECT_EQ(sorted_dups[0].name, "Alice");
    EXPECT_EQ(sorted_dups[1].name, "Bob");
    EXPECT_EQ(sorted_dups[2].name, "Charlie");
}

// Test for pipe functionality
TEST(OrderBy, PipeFunction) {
    MSG("Testing pipe functionality");

    df::Serie<double> values{5.0, 2.0, 8.0, 1.0, 9.0};

    // Using pipe syntax
    auto result = values | df::bind_orderBy([](double x) { return x; });
    df::Serie<double> expected{1.0, 2.0, 5.0, 8.0, 9.0};
    COMPARE_SERIES(result, expected);

    // Multiple operations with pipe
    auto complex_result =
        values |
        df::bind_orderBy([](double x) { return x; }, false) // Sort descending
        | df::bind_map([](double x) { return x * 2; });     // Double each value

    df::Serie<double> expected_complex{18.0, 16.0, 10.0, 4.0, 2.0};
    COMPARE_SERIES(complex_result, expected_complex);
}

// Test with vectors and arrays
TEST(OrderBy, VectorsAndArrays) {
    MSG("Testing with vectors and arrays");

    // Create a test serie of Vector2
    df::Serie<Vector2> vectors{
        {3.0, 4.0}, // length = 5
        {1.0, 1.0}, // length = 1.414
        {5.0, 0.0}, // length = 5
        {0.0, 2.0}, // length = 2
        {2.0, 2.0}  // length = 2.828
    };

    // Sort by vector length
    auto by_length = df::orderBy(vectors, [](const Vector2 &v) {
        return std::sqrt(v[0] * v[0] + v[1] * v[1]);
    });

    df::Serie<Vector2> expected{
        {1.0, 1.0}, {0.0, 2.0}, {2.0, 2.0}, {3.0, 4.0}, {5.0, 0.0}};

    EXPECT_EQ(by_length.size(), expected.size());
    for (size_t i = 0; i < by_length.size(); ++i) {
        EXPECT_ARRAY_NEAR(by_length[i], expected[i], 1e-6);
    }

    // Sort by just the first component
    auto by_first = df::orderBy(vectors, [](const Vector2 &v) { return v[0]; });
    df::Serie<Vector2> expected_first{
        {0.0, 2.0}, {1.0, 1.0}, {2.0, 2.0}, {3.0, 4.0}, {5.0, 0.0}};

    EXPECT_EQ(by_first.size(), expected_first.size());
    for (size_t i = 0; i < by_first.size(); ++i) {
        EXPECT_ARRAY_NEAR(by_first[i], expected_first[i], 1e-6);
    }
}

// Main function to run all tests
RUN_TESTS()