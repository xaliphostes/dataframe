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
#include <dataframe/core/format.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/types.h>
#include <dataframe/utils/utils.h>

TEST(Concat, BasicUsage) {
    // Basic types
    auto result1 = concat("Value: ", 42);
    EXPECT_STREQ(result1, "Value: 42");

    // Multiple types
    auto result2 = concat("Pi: ", 3.14159, ", e: ", 2.71828);
    EXPECT_STREQ(result2, "Pi: 3.14159, e: 2.71828");

    // With arrays
    Vector2 v{1.5, 2.5};
    auto result3 = concat("Vector: ", v);
    EXPECT_STREQ(result3, "Vector: [1.5,2.5]");
}

TEST(Concat, SerieConcat) {
    // Simple serie
    df::Serie<int> numbers{1, 2, 3, 4, 5};

    // Default separator
    auto result1 = df::format(numbers, "", ", ", "");
    EXPECT_STREQ(result1, "1, 2, 3, 4, 5");

    // Custom separator
    auto result2 = df::format(numbers, "", " | ", "");
    EXPECT_STREQ(result2, "1 | 2 | 3 | 4 | 5");

    // Empty serie
    df::Serie<int> empty{};
    auto result3 = df::format(empty, "", ", ", "");
    EXPECT_STREQ(result3, "");
}

TEST(Concat, FormatSerie) {
    df::Serie<double> values{1.1, 2.2, 3.3};

    // Default formatting
    auto result1 = df::format(values);
    EXPECT_STREQ(result1, "[1.1, 2.2, 3.3]");

    // Custom formatting
    auto result2 = df::format(values, "{ ", "; ", " }");
    EXPECT_STREQ(result2, "{ 1.1; 2.2; 3.3 }");

    // Empty serie
    df::Serie<double> empty{};
    auto result3 = df::format(empty);
    EXPECT_STREQ(result3, "[]");
}

TEST(Concat, PipeUsage) {
    // Simple pipe
    auto result1 = "Hello" | df::bind_format(" World");
    EXPECT_STREQ(result1, "Hello World");

    // Multiple operations in pipe
    auto result2 =
        "Count: " | df::bind_format(5) | df::bind_format(", Value: ", 3.14);
    EXPECT_STREQ(result2, "Count: 5, Value: 3.14");

    // With serie
    df::Serie<int> serie{10, 20, 30};
    auto result3 = std::string("Serie: ") | df::bind_format(df::format(serie));
    EXPECT_STREQ(result3, "Serie: [10, 20, 30]");
}

// TEST(Concat, OperatorUsage) {
//     // Basic operator usage
//     auto result1 = "Hello" + " World";
//     EXPECT_STREQ(result1, "Hello World");

//     // Different types
//     auto result2 = "Number: " ^ 42;
//     EXPECT_STREQ(result2, "Number: 42");

//     // Chaining operators
//     auto result3 = "A" ^ "B" ^ "C" ^ 123;
//     EXPECT_STREQ(result3, "ABC123");

//     // With complex types
//     Vector3 v{1.0, 2.0, 3.0};
//     auto result4 = "Vector: " ^ v;
//     EXPECT_STREQ(result4, "Vector: [1,2,3]");
// }

RUN_TESTS()