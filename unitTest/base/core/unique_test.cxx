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
#include <dataframe/core/pipe.h>
#include <dataframe/core/unique.h>
#include <string>

// Define a simple struct for testing complex types
struct Person {
    std::string name;
    int age;

    // Define equality operator for testing
    bool operator==(const Person &other) const {
        return name == other.name && age == other.age;
    }
};

// Custom hash for Person to enable hash-based uniqueness checking
namespace std {
template <> struct hash<Person> {
    size_t operator()(const Person &p) const {
        return hash<string>()(p.name) ^ hash<int>()(p.age);
    }
};
} // namespace std

// Output operator for Person, useful for test failure messages
std::ostream &operator<<(std::ostream &os, const Person &p) {
    os << "Person{name=\"" << p.name << "\", age=" << p.age << "}";
    return os;
}

// Test for standard unique functionality with primitive types
TEST(Unique, PrimitiveTypes) {
    MSG("Testing unique with primitive types");

    // Integers
    df::Serie<int> ints{1, 2, 3, 2, 1, 4, 5, 3};
    auto unique_ints = df::unique(ints);
    df::Serie<int> expected_ints{1, 2, 3, 4, 5};
    COMPARE_SERIES(unique_ints, expected_ints);

    // Doubles
    df::Serie<double> doubles{1.1, 2.2, 3.3, 2.2, 1.1, 4.4, 5.5, 3.3};
    auto unique_doubles = df::unique(doubles);
    df::Serie<double> expected_doubles{1.1, 2.2, 3.3, 4.4, 5.5};
    COMPARE_SERIES(unique_doubles, expected_doubles);

    // Strings
    df::Serie<std::string> strings{"a", "b", "c", "b", "a", "d", "e", "c"};
    auto unique_strings = df::unique(strings);
    df::Serie<std::string> expected_strings{"a", "b", "c", "d", "e"};
    COMPARE_SERIES(unique_strings, expected_strings);
}

// Test for unique with custom objects
TEST(Unique, ComplexObjects) {
    MSG("Testing unique with complex objects");

    df::Serie<Person> people{{"Alice", 30},   {"Bob", 25},
                             {"Charlie", 40}, {"Alice", 30}, // Duplicate
                             {"David", 35},   {"Bob", 25},   // Duplicate
                             {"Eve", 28}};

    auto unique_people = df::unique(people);
    df::Serie<Person> expected_people{{"Alice", 30},
                                      {"Bob", 25},
                                      {"Charlie", 40},
                                      {"David", 35},
                                      {"Eve", 28}};

    COMPARE_SERIES(unique_people, expected_people);
}

// Test for unique_by with a key function
TEST(Unique, UniqueByKeyFunction) {
    MSG("Testing unique_by with key function");

    // Using unique_by with integers and absolute value
    df::Serie<int> ints{1, -1, 2, -2, 3, 4, -4, 5};
    auto unique_abs = df::unique_by(ints, [](int x) { return std::abs(x); });
    df::Serie<int> expected_abs{1, 2, 3, 4, 5};
    COMPARE_SERIES(unique_abs, expected_abs);

    // Using unique_by with Person objects and age
    df::Serie<Person> people{{"Alice", 30}, {"Bob", 25}, {"Charlie", 40},
                             {"David", 30}, // Same age as Alice
                             {"Eve", 25},   // Same age as Bob
                             {"Frank", 35}};

    auto unique_by_age =
        df::unique_by(people, [](const Person &p) { return p.age; });
    df::Serie<Person> expected_by_age{
        {"Alice", 30}, {"Bob", 25}, {"Charlie", 40}, {"Frank", 35}};

    COMPARE_SERIES(unique_by_age, expected_by_age);

    // Using unique_by with Person objects and name first letter
    auto unique_by_first_letter = df::unique_by(people, [](const Person &p) {
        return p.name.empty() ? '\0' : p.name[0];
    });

    df::Serie<Person> expected_by_letter{{"Alice", 30},   {"Bob", 25},
                                         {"Charlie", 40}, {"David", 30},
                                         {"Eve", 25},     {"Frank", 35}};

    COMPARE_SERIES(unique_by_first_letter, expected_by_letter);
}

// Test for edge cases
TEST(Unique, EdgeCases) {
    MSG("Testing edge cases");

    // Empty series
    df::Serie<int> empty;
    auto unique_empty = df::unique(empty);
    EXPECT_EQ(unique_empty.size(), 0);

    // Single element series
    df::Serie<int> single{42};
    auto unique_single = df::unique(single);
    EXPECT_EQ(unique_single.size(), 1);
    EXPECT_EQ(unique_single[0], 42);

    // Series with all duplicates
    df::Serie<int> all_same{5, 5, 5, 5, 5};
    auto unique_all_same = df::unique(all_same);
    EXPECT_EQ(unique_all_same.size(), 1);
    EXPECT_EQ(unique_all_same[0], 5);

    // Series with no duplicates
    df::Serie<int> no_dups{1, 2, 3, 4, 5};
    auto unique_no_dups = df::unique(no_dups);
    COMPARE_SERIES(unique_no_dups, no_dups);
}

// Test for pipe functionality
TEST(Unique, PipeFunction) {
    MSG("Testing pipe functionality");

    df::Serie<int> ints{1, 2, 3, 2, 1, 4, 5, 3};

    // Basic pipe usage
    auto result = ints | df::bind_unique<int>();
    df::Serie<int> expected{1, 2, 3, 4, 5};
    COMPARE_SERIES(result, expected);

    // Complex pipe usage
    auto complex_result =
        ints | df::bind_map([](int x) { return x * 2; }) // Double each value
        | df::bind_unique<int>();                        // Remove duplicates

    df::Serie<int> expected_complex{2, 4, 6, 8, 10};
    COMPARE_SERIES(complex_result, expected_complex);

    // Pipe with unique_by
    auto unique_by_result = ints | df::bind_unique_by([](int x) {
                                return x % 2;
                            }); // Keep first odd and even

    df::Serie<int> expected_by{1, 2};
    COMPARE_SERIES(unique_by_result, expected_by);
}

// Test with Vector type
TEST(Unique, VectorType) {
    MSG("Testing with Vector type");

    df::Serie<Vector2> vectors{
        {1.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, // Duplicate
        {0.0, 2.0}, {2.0, 0.0}, {0.0, 1.0}  // Duplicate
    };

    auto unique_vectors = df::unique(vectors);
    df::Serie<Vector2> expected{{1.0, 0.0}, {0.0, 1.0}, {0.0, 2.0}, {2.0, 0.0}};

    EXPECT_EQ(unique_vectors.size(), expected.size());
    for (size_t i = 0; i < unique_vectors.size(); ++i) {
        EXPECT_ARRAY_EQ(unique_vectors[i], expected[i]);
    }

    // Test unique_by with magnitude
    auto unique_by_magnitude = df::unique_by(vectors, [](const Vector2 &v) {
        return std::sqrt(v[0] * v[0] + v[1] * v[1]);
    });

    df::Serie<Vector2> expected_magnitude{
        {1.0, 0.0}, // magnitude 1
        {0.0, 2.0}, // magnitude 2
    };

    EXPECT_EQ(unique_by_magnitude.size(), expected_magnitude.size());
    for (size_t i = 0; i < unique_by_magnitude.size(); ++i) {
        EXPECT_ARRAY_EQ(unique_by_magnitude[i], expected_magnitude[i]);
    }
}

// Test performance comparison between large datasets
TEST(Unique, Performance) {
    MSG("Testing performance");

    const size_t SIZE = 10000;

    // Generate test data with 50% duplicates
    std::vector<int> data;
    data.reserve(SIZE);
    for (size_t i = 0; i < SIZE; ++i) {
        data.push_back(i % (SIZE / 2));
    }
    df::Serie<int> large_serie(data);

    // Time the unique operation
    double time_ms = TIMING([&]() {
        auto result = df::unique(large_serie);
        EXPECT_EQ(result.size(), SIZE / 2);
    });

    MSG("Unique operation on ", SIZE, " elements took ", time_ms, " ms");
}

// Main function to run all tests
RUN_TESTS()