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
#include <dataframe/core/flatten.h>
#include <dataframe/core/pipe.h>
#include <list>
#include <string>
#include <vector>

TEST(flatten, SerieOfVectors) {
    // Create a Serie of vectors
    std::vector<int> v1{1, 2, 3};
    std::vector<int> v2{4, 5};
    std::vector<int> v3{6, 7, 8, 9};

    df::Serie<std::vector<int>> nested{v1, v2, v3};

    // Flatten the Serie
    auto flattened = df::flatten(nested);
    df::print(flattened);

    // Check the result
    EXPECT_EQ(flattened.size(), 9);
    EXPECT_ARRAY_EQ(flattened.asArray(),
                    std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9}));
}

TEST(flatten, SerieOfSeries) {
    // Create a Serie of Series
    df::Serie<double> s1{1.1, 2.2};
    df::Serie<double> s2{3.3, 4.4, 5.5};

    df::Serie<df::Serie<double>> series_of_series{s1, s2};

    // Flatten the Serie
    auto flattened = df::flatten(series_of_series);
    df::print(flattened);

    // Check the result
    EXPECT_EQ(flattened.size(), 5);
    EXPECT_ARRAY_NEAR(flattened.asArray(),
                      std::vector<double>({1.1, 2.2, 3.3, 4.4, 5.5}), 0.001);
}

TEST(flatten, MixedNestedTypes) {
    // Create a Serie of std::vectors and std::lists
    std::vector<int> v1{1, 2};
    std::list<int> l1{3, 4, 5};
    std::vector<int> v2{6, 7, 8};

    df::Serie<std::vector<int>> serie_vec{v1, v2};

    // Flatten the Serie
    auto flattened_vec = df::flatten(serie_vec);
    df::print(flattened_vec);

    // Check the result
    EXPECT_EQ(flattened_vec.size(), 5);
    EXPECT_ARRAY_EQ(flattened_vec.asArray(), std::vector<int>({1, 2, 6, 7, 8}));

    // This won't compile because the nested types are different:
    // df::Serie<void*> mixed_serie{&v1, &l1, &v2};
    // auto flattened_mixed = df::flatten(mixed_serie);
}

// TEST(flatten, DeeplyNested) {
//     // Create deeply nested structure: Serie of vector of vector of int
//     std::vector<std::vector<int>> vv1{{1, 2}, {3, 4}};
//     std::vector<std::vector<int>> vv2{{5, 6}, {7, 8}};

//     df::Serie<std::vector<std::vector<int>>> deeply_nested{vv1, vv2};

//     // Flatten one level
//     auto partially_flat = df::flattenDeep(deeply_nested, 1);

//     // The result should be a Serie<std::vector<int>>
//     EXPECT_EQ(partially_flat.size(), 4);

//     // We need to check each vector individually
//     EXPECT_ARRAY_EQ(partially_flat[0].asArray(), std::vector<int>({1, 2}));
//     EXPECT_ARRAY_EQ(partially_flat[1].asArray(), std::vector<int>({3, 4}));
//     EXPECT_ARRAY_EQ(partially_flat[2].asArray(), std::vector<int>({5, 6}));
//     EXPECT_ARRAY_EQ(partially_flat[3].asArray(), std::vector<int>({7, 8}));

//     // Flatten all levels
//     auto completely_flat = df::flattenDeep(deeply_nested);

//     // Check the result
//     EXPECT_EQ(completely_flat.size(), 8);
//     EXPECT_ARRAY_EQ(completely_flat.asArray(),
//                     std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8}));
// }

// TEST(flatten, WithPipe) {
//     // Create a Serie of vectors
//     std::vector<int> v1{1, 2, 3};
//     std::vector<int> v2{4, 5};
//     std::vector<int> v3{6, 7, 8, 9};

//     df::Serie<std::vector<int>> nested{v1, v2, v3};

//     // Flatten using pipe syntax
//     auto flattened = nested | df::bind_flatten();

//     // Check the result
//     EXPECT_EQ(flattened.size(), 9);
//     EXPECT_ARRAY_EQ(flattened.asArray(),
//                     std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8, 9}));

//     // Create deeply nested structure
//     std::vector<std::vector<int>> vv1{{1, 2}, {3, 4}};
//     std::vector<std::vector<int>> vv2{{5, 6}, {7, 8}};

//     df::Serie<std::vector<std::vector<int>>> deeply_nested{vv1, vv2};

//     // Flatten one level using pipe syntax
//     auto partially_flat = deeply_nested | df::bind_flattenDeep(1);

//     // The result should be a Serie<std::vector<int>>
//     EXPECT_EQ(partially_flat.size(), 4);

//     // Flatten all levels using pipe syntax
//     auto completely_flat = deeply_nested | df::bind_flattenDeep();

//     // Check the result
//     EXPECT_EQ(completely_flat.size(), 8);
//     EXPECT_ARRAY_EQ(completely_flat.asArray(),
//                     std::vector<int>({1, 2, 3, 4, 5, 6, 7, 8}));
// }

TEST(flatten, EdgeCases) {
    // Test with empty Serie
    df::Serie<std::vector<int>> empty_serie;
    auto flattened_empty = df::flatten(empty_serie);

    EXPECT_EQ(flattened_empty.size(), 0);

    // Test with Serie of empty containers
    std::vector<int> empty_vec;
    df::Serie<std::vector<int>> serie_of_empty{empty_vec, empty_vec, empty_vec};

    auto flattened_empty_containers = df::flatten(serie_of_empty);

    EXPECT_EQ(flattened_empty_containers.size(), 0);

    // Test with Serie of mixed empty and non-empty containers
    df::Serie<std::vector<int>> mixed_serie{
        empty_vec, {1, 2, 3}, empty_vec, {4, 5}};

    auto flattened_mixed = df::flatten(mixed_serie);

    EXPECT_EQ(flattened_mixed.size(), 5);
    EXPECT_ARRAY_EQ(flattened_mixed.asArray(),
                    std::vector<int>({1, 2, 3, 4, 5}));

    // Test flattening strings (should not try to iterate characters)
    df::Serie<std::string> strings{"hello", "world"};
    auto flattened_strings = df::flatten(strings);

    EXPECT_EQ(flattened_strings.size(), 2);
    EXPECT_STREQ(flattened_strings[0].c_str(), "hello");
    EXPECT_STREQ(flattened_strings[1].c_str(), "world");
}

// Run the tests
RUN_TESTS()
