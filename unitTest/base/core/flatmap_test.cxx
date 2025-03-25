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
#include <dataframe/core/flatmap.h>
#include <dataframe/core/pipe.h>
#include <string>
#include <vector>

TEST(FlatMap, BasicFlatMap) {
    // Create a Serie of strings
    df::Serie<std::string> words{"hello", "world", "dataframe"};

    // FlatMap to get characters
    auto characters = df::flatMap<std::string, char>(words, [](const std::string& word, size_t) {
        std::vector<char> chars(word.begin(), word.end());
        return df::Serie<char>(chars);
    });

    // Check the result
    EXPECT_EQ(characters.size(), 19); // "hello" + "world" + "dataframe" = 5 + 5 + 9 = 19
    EXPECT_EQ(characters[0], 'h');
    EXPECT_EQ(characters[5], 'w');
    EXPECT_EQ(characters[10], 'd');
}

TEST(FlatMap, FlatMapWithoutIndex) {
    // Create a Serie of numbers
    df::Serie<int> numbers{1, 2, 3};

    // FlatMap to repeat each number according to its value
    auto repeated = df::flatMap<int, int>(numbers, [](int n) {
        std::vector<int> repeats(n, n);
        return df::Serie<int>(repeats);
    });

    // Check the result
    EXPECT_EQ(repeated.size(), 6); // 1 + 2 + 3 = 6 elements
    EXPECT_EQ(repeated[0], 1);
    EXPECT_EQ(repeated[1], 2);
    EXPECT_EQ(repeated[2], 2);
    EXPECT_EQ(repeated[3], 3);
    EXPECT_EQ(repeated[4], 3);
    EXPECT_EQ(repeated[5], 3);
}

TEST(FlatMap, EmptySeries) {
    // Create an empty Serie
    df::Serie<int> empty;

    // FlatMap should return empty Serie
    auto result = df::flatMap<int, std::string>(empty, [](int n, size_t) {
        return df::Serie<std::string>{"dummy"};
    });

    EXPECT_TRUE(result.empty());
}

TEST(FlatMap, PipelineUsage) {
    // Create a Serie of numbers
    df::Serie<int> numbers{1, 2, 3, 4};

    // Create a pipeline with flatMap
    auto result = numbers 
        | df::bind_flatMap<int, std::string>([](int n, size_t idx) {
            std::vector<std::string> result;
            for (int i = 0; i < n; ++i) {
                result.push_back("Value " + std::to_string(n) + " at index " + std::to_string(idx));
            }
            return df::Serie<std::string>(result);
        });

    // Check the result
    EXPECT_EQ(result.size(), 10); // 1 + 2 + 3 + 4 = 10 strings
    EXPECT_EQ(result[0], "Value 1 at index 0");
    EXPECT_EQ(result[1], "Value 2 at index 1");
    EXPECT_EQ(result[2], "Value 2 at index 1");
}

TEST(FlatMap, ComplexTransformation) {
    // Create a Serie of string sentences
    df::Serie<std::string> sentences{
        "Hello world",
        "This is a test",
        "DataFrame library"
    };

    // Split each sentence into words
    auto words = df::flatMap<std::string, std::string>(sentences, [](const std::string& sentence, size_t) {
        std::vector<std::string> words;
        std::string word;
        for (char c : sentence) {
            if (c == ' ') {
                if (!word.empty()) {
                    words.push_back(word);
                    word.clear();
                }
            } else {
                word += c;
            }
        }
        if (!word.empty()) {
            words.push_back(word);
        }
        return df::Serie<std::string>(words);
    });

    // Check the result
    EXPECT_EQ(words.size(), 8);
    EXPECT_EQ(words[0], "Hello");
    EXPECT_EQ(words[1], "world");
    EXPECT_EQ(words[2], "This");
    EXPECT_EQ(words[3], "is");
    EXPECT_EQ(words[4], "a");
    EXPECT_EQ(words[5], "test");
    EXPECT_EQ(words[6], "DataFrame");
}

RUN_TESTS()