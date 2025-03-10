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
#include <dataframe/Serie.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/core/switch.h>
#include <string>

TEST(Switch, BasicSwitch) {
    auto series = df::Serie<int>{95, 82, 67, 45, 78};

    // Convert numeric scores to letter grades
    auto letter_grades = df::switch_case<int, std::string>(
        series,
        {{[](const int &x) { return x >= 90; },
          [](const int &) { return "A"; }},
         {[](const int &x) { return x >= 80; },
          [](const int &) { return "B"; }},
         {[](const int &x) { return x >= 70; },
          [](const int &) { return "C"; }},
         {[](const int &x) { return x >= 60; },
          [](const int &) { return "D"; }}},
        [](const int &) { return "F"; });

    std::vector<std::string> expected = {"A", "B", "D", "F", "C"};
    COMPARE_SERIE_VECTOR(letter_grades, expected);
}

TEST(Switch, IndexAwareSwitch) {
    auto series = df::Serie<int>{10, 20, 30, 40, 50};

    // Use switch with index-aware conditions and transformations
    auto result = df::switch_case<int, std::string>(
        series,
        {{[](const int &x) { return x > 30; },
          [](const int &val) { return "High: " + std::to_string(val); }},
         {[](const int &x) { return x < 20; },
          [](const int &val) { return "Low: " + std::to_string(val); }}},
        [](const int &val) { return "Mid: " + std::to_string(val); });

    std::vector<std::string> expected = {"Low: 10", "Mid: 20", "Mid: 30",
                                         "High: 40", "High: 50"};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Switch, EmptySeries) {
    auto empty_series = df::Serie<int>{};

    // Switch on empty series should return empty series
    auto result = df::switch_case<int, std::string>(
        empty_series,
        {{[](const int &x) { return x > 0; },
          [](const int &) { return "Positive"; }}},
        [](const int &) { return "Zero or Negative"; });

    EXPECT_EQ(result.size(), 0);
}

TEST(Switch, NoMatchingCases) {
    auto series = df::Serie<int>{-5, -10, -15};

    // No case matches, should use default case for all
    auto result = df::switch_case<int, std::string>(
        series,
        {{[](const int &x) { return x > 0; },
          [](const int &) { return "Positive"; }},
         {[](const int &x) { return x == 0; },
          [](const int &) { return "Zero"; }}},
        [](const int &val) { return "Negative: " + std::to_string(val); });

    std::vector<std::string> expected = {"Negative: -5", "Negative: -10",
                                         "Negative: -15"};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Switch, ComplexTypes) {
    auto vectors = df::Serie<Vector3>{
        {1, 0, 0}, {0, 2, 0}, {0, 0, 3}, {1, 1, 1}, {2, 2, 2}};

    // Categorize vectors based on their properties
    auto categories = df::switch_case<Vector3, std::string>(
        vectors,
        {{[](const Vector3 &v) {
              return (v[0] == 0 && v[1] == 0) || (v[0] == 0 && v[2] == 0) ||
                     (v[1] == 0 && v[2] == 0);
          },
          [](const Vector3 &) { return "Axis aligned"; }},

         {[](const Vector3 &v) {
              double magnitude =
                  std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
              return magnitude > 2.5;
          },
          [](const Vector3 &) { return "Large magnitude"; }}},
        [](const Vector3 &) { return "Other"; });

    std::vector<std::string> expected = {"Axis aligned", "Axis aligned",
                                         "Axis aligned", "Other",
                                         "Large magnitude"};
    COMPARE_SERIE_VECTOR(categories, expected);
}

TEST(Switch, TypeConversion) {
    auto series = df::Serie<double>{-10.5, 0.0, 3.14, 2.718, 100.0};

    // Convert to different numeric type
    auto result = df::switch_case<double, int>(
        series,
        {{[](const double &x) { return x < 0; },
          [](const double &x) { return static_cast<int>(std::floor(x)); }},
         {[](const double &x) { return x > 10; },
          [](const double &x) { return static_cast<int>(std::ceil(x)); }}},
        [](const double &x) { return static_cast<int>(std::round(x)); });

    std::vector<int> expected = {-11, 0, 3, 3, 100};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Switch, PipeUsage) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Test switch in pipe
    auto result =
        series |
        df::bind_switch_case<int, std::string>(
            {{[](const int &x) { return x % 2 == 0; },
              [](const int &x) { return "Even: " + std::to_string(x); }},
             {[](const int &x) { return x > 5; },
              [](const int &x) { return "Large: " + std::to_string(x); }}},
            [](const int &x) { return "Other: " + std::to_string(x); });

    std::vector<std::string> expected = {
        "Other: 1", "Even: 2",  "Other: 3", "Even: 4",  "Other: 5",
        "Even: 6",  "Large: 7", "Even: 8",  "Large: 9", "Even: 10"};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Switch, CombiningWithOtherOperations) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5};

    // Define a reusable pipeline that combines map and switch_case
    auto pipeline = df::make_pipe(
        df::bind_map([](int x, size_t) { return x * 2; }),
        df::bind_switch_case<int, std::string>(
            {{[](const int &x) { return x % 2 == 0 && x <= 6; },
              [](const int &x) { return "Small even: " + std::to_string(x); }},
             {[](const int &x) { return x % 2 == 0; },
              [](const int &x) { return "Large even: " + std::to_string(x); }}},
            [](const int &x) { return "Other: " + std::to_string(x); }));

    // Apply the pipeline to the series
    auto result = pipeline(series);

    std::vector<std::string> expected = {"Small even: 2", "Small even: 4",
                                         "Small even: 6", "Large even: 8",
                                         "Large even: 10"};
    COMPARE_SERIE_VECTOR(result, expected);
}

RUN_TESTS()