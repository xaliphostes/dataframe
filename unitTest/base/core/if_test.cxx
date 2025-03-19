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
 */

#include "../../TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/if.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <string>

TEST(If, BasicIfThenElse) {
    auto numbers = df::Serie<double>{-5.0, 3.2, -1.7, 8.1, 0.0};

    // Replace negative values with 0
    auto zeroed_negatives = df::if_then_else<double, double>(
        numbers, [](double x) { return x < 0; }, [](double) { return 0.0; },
        [](double x) { return x; });

    auto expected = df::Serie<double>{0.0, 3.2, 0.0, 8.1, 0.0};
    COMPARE_SERIES(zeroed_negatives, expected);

    // Calculate absolute values
    auto absolute_values = df::if_then_else<double, double>(
        numbers, [](double x) { return x < 0; }, [](double x) { return -x; },
        [](double x) { return x; });

    auto expected_abs = df::Serie<double>{5.0, 3.2, 1.7, 8.1, 0.0};
    COMPARE_SERIES(absolute_values, expected_abs);
}

TEST(If, TypeConversion) {
    auto numbers = df::Serie<int>{-5, 3, -2, 8, 0};

    // Convert to strings with sign indicators
    auto signed_strings = df::if_then_else<int, std::string>(
        numbers, [](int x) { return x < 0; },
        [](int x) { return "neg:" + std::to_string(x); },
        [](int x) { return "pos:" + std::to_string(x); });

    std::vector<std::string> expected_vector = {"neg:-5", "pos:3", "neg:-2",
                                                "pos:8", "pos:0"};
    auto expected = df::Serie<std::string>(expected_vector);
    COMPARE_SERIES(signed_strings, expected);
}

TEST(If, WithIndex) {
    auto numbers = df::Serie<int>{10, 20, 30, 40, 50};

    // Apply different transformations based on index
    auto result = df::if_then_else<int, int>(
        numbers,
        [](int, size_t idx) { return idx % 2 == 0; }, // Condition on index
        [](int x, size_t) { return x * 2; }, // Double even-indexed values
        [](int x, size_t) { return x / 2; }  // Halve odd-indexed values
    );

    auto expected = df::Serie<int>{20, 10, 60, 20, 100};
    COMPARE_SERIES(result, expected);
}

TEST(If, EmptySeries) {
    auto empty_series = df::Serie<double>{};

    // Operation on empty series should return empty series
    auto result = df::if_then_else<double, double>(
        empty_series, [](double x) { return x < 0; },
        [](double x) { return -x; }, [](double x) { return x; });

    EXPECT_TRUE(result.empty());
}

TEST(If, ComplexTypes) {
    auto vectors = df::Serie<Vector3>{
        {1, 0, 0}, {0, 2, 0}, {0, 0, 3}, {1, 1, 1}, {2, 2, 2}};

    // Normalize vectors with magnitude >= 2, leave others unchanged
    auto processed = df::if_then_else<Vector3, Vector3>(
        vectors,
        [](const Vector3 &v) {
            double mag2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
            return mag2 >= 4.0;
        },
        [](const Vector3 &v) {
            // Normalize the vector
            double mag = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            return Vector3{v[0] / mag, v[1] / mag, v[2] / mag};
        },
        [](const Vector3 &v) {
            // Return unchanged
            return v;
        });

    // Expected results (vectors with magnitude > 2 are normalized)
    auto expected = df::Serie<Vector3>{
        {1, 0, 0}, // unchanged
        {0, 1, 0}, // normalized {0, 2, 0} -> {0, 1, 0}
        {0, 0, 1}, // normalized {0, 0, 3} -> {0, 0, 1}
        {1, 1, 1}, // unchanged
        {2.0 / std::sqrt(12.0), 2.0 / std::sqrt(12.0),
         2.0 / std::sqrt(12.0)} // normalized
    };

    // Compare each vector component with tolerance for floating point
    // comparisons
    for (size_t i = 0; i < processed.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_NEAR(processed[i][j], expected[i][j], 1e-10);
        }
    }
}

TEST(If, PipeUsage) {
    auto numbers = df::Serie<double>{-5.0, 3.2, -1.7, 8.1, 0.0};

    // Using bind_if_then_else in a pipeline
    auto absolute_values =
        numbers | df::bind_if_then_else<double, double>(
                      [](double x) { return x < 0; },
                      [](double x) { return -x; }, [](double x) { return x; });

    auto expected = df::Serie<double>{5.0, 3.2, 1.7, 8.1, 0.0};
    COMPARE_SERIES(absolute_values, expected);

    // Multiple operations in a pipeline
    auto result = numbers |
                  df::bind_if_then_else<double, double>(
                      [](double x) { return x < 0; },
                      [](double x) { return -x; }, // Take absolute value
                      [](double x) { return x; }) |
                  df::bind_map([](double x, size_t) {
                      return x * 2;
                  }); // Double the result

    auto expected_doubled = df::Serie<double>{10.0, 6.4, 3.4, 16.2, 0.0};
    COMPARE_SERIES(result, expected_doubled);
}

TEST(If, ChainedConditions) {
    auto numbers = df::Serie<int>{-10, -5, 0, 5, 10, 15, 20};

    // Chain multiple if_then_else operations using bind
    auto categorized = numbers | df::bind_if_then_else<int, std::string>(
                                     [](int x) { return x < 0; },
                                     [](int) { return "negative"; },
                                     [](int x) {
                                         return x > 0 ? "positive" : "zero";
                                     } // Nested condition
                                 );

    auto expected_vector =
        std::vector<std::string>{"negative", "negative", "zero",    "positive",
                                 "positive", "positive", "positive"};
    auto expected = df::Serie<std::string>(expected_vector);
    COMPARE_SERIES(categorized, expected);

    // More complex categorization using multiple if_then_else
    auto graded =
        numbers | df::bind_if_then_else<int, std::string>(
                      [](int x) { return x < 0; }, [](int) { return "F"; },
                      [](int x) {
                          // Nested condition
                          if (x == 0)
                              return "D";
                          else if (x < 10)
                              return "C";
                          else if (x < 15)
                              return "B";
                          else
                              return "A";
                      });

    auto expected_grades =
        std::vector<std::string>{"F", "F", "D", "C", "B", "A", "A"};
    auto expected_graded = df::Serie<std::string>(expected_grades);
    COMPARE_SERIES(graded, expected_graded);
}

RUN_TESTS()