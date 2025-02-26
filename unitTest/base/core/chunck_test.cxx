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
#include <dataframe/core/pipe.h>
#include <dataframe/core/chunk.h>

TEST(Utils, ChunkSingleSerie) {
    // Test with evenly divisible series
    df::Serie<int> serie1{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto chunks1 = df::chunk(3, serie1);
    
    EXPECT_EQ(chunks1.size(), 3);
    EXPECT_EQ(chunks1[0].size(), 3);
    EXPECT_EQ(chunks1[1].size(), 3);
    EXPECT_EQ(chunks1[2].size(), 3);
    
    EXPECT_EQ(chunks1[0][0], 1);
    EXPECT_EQ(chunks1[0][1], 2);
    EXPECT_EQ(chunks1[0][2], 3);
    
    EXPECT_EQ(chunks1[1][0], 4);
    EXPECT_EQ(chunks1[1][1], 5);
    EXPECT_EQ(chunks1[1][2], 6);
    
    EXPECT_EQ(chunks1[2][0], 7);
    EXPECT_EQ(chunks1[2][1], 8);
    EXPECT_EQ(chunks1[2][2], 9);
    
    // Test with non-evenly divisible series
    df::Serie<int> serie2{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto chunks2 = df::chunk(3, serie2);
    
    EXPECT_EQ(chunks2.size(), 4);
    EXPECT_EQ(chunks2[0].size(), 3);
    EXPECT_EQ(chunks2[1].size(), 3);
    EXPECT_EQ(chunks2[2].size(), 3);
    EXPECT_EQ(chunks2[3].size(), 1);
    
    EXPECT_EQ(chunks2[3][0], 10);
    
    // Test with chunk size larger than series
    df::Serie<int> serie3{1, 2, 3};
    auto chunks3 = df::chunk(5, serie3);
    
    EXPECT_EQ(chunks3.size(), 1);
    EXPECT_EQ(chunks3[0].size(), 3);
    
    // Test with pipe syntax
    auto chunks4 = serie1 | df::bind_chunk<int>(3);
    EXPECT_EQ(chunks4.size(), 3);
}

TEST(Utils, ChunkMultipleSeries) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8};
    df::Serie<std::string> labels{"a", "b", "c", "d", "e", "f", "g", "h"};
    
    auto chunks = df::chunk(3, numbers, labels);
    
    EXPECT_EQ(chunks.size(), 3);
    
    // First chunk
    auto [chunk1_nums, chunk1_labels] = chunks[0];
    EXPECT_EQ(chunk1_nums.size(), 3);
    EXPECT_EQ(chunk1_labels.size(), 3);
    
    EXPECT_EQ(chunk1_nums[0], 1);
    EXPECT_EQ(chunk1_nums[1], 2);
    EXPECT_EQ(chunk1_nums[2], 3);
    
    EXPECT_STREQ(chunk1_labels[0].c_str(), "a");
    EXPECT_STREQ(chunk1_labels[1].c_str(), "b");
    EXPECT_STREQ(chunk1_labels[2].c_str(), "c");
    
    // Second chunk
    auto [chunk2_nums, chunk2_labels] = chunks[1];
    EXPECT_EQ(chunk2_nums.size(), 3);
    EXPECT_EQ(chunk2_labels.size(), 3);
    
    EXPECT_EQ(chunk2_nums[0], 4);
    EXPECT_EQ(chunk2_nums[1], 5);
    EXPECT_EQ(chunk2_nums[2], 6);
    
    // Last chunk (partial)
    auto [chunk3_nums, chunk3_labels] = chunks[2];
    EXPECT_EQ(chunk3_nums.size(), 2);
    EXPECT_EQ(chunk3_labels.size(), 2);
    
    EXPECT_EQ(chunk3_nums[0], 7);
    EXPECT_EQ(chunk3_nums[1], 8);
}

TEST(Utils, ChunkEdgeCases) {
    // Test with empty series
    df::Serie<double> empty_serie;
    auto empty_chunks = df::chunk(3, empty_serie);
    
    EXPECT_EQ(empty_chunks.size(), 0);
    
    // Test with chunk size of 1
    df::Serie<int> serie{1, 2, 3};
    auto chunks = df::chunk(1, serie);
    
    EXPECT_EQ(chunks.size(), 3);
    EXPECT_EQ(chunks[0].size(), 1);
    EXPECT_EQ(chunks[1].size(), 1);
    EXPECT_EQ(chunks[2].size(), 1);
    
    // Test invalid chunk size (should throw)
    EXPECT_THROW(df::chunk(0, serie), std::invalid_argument);
    
    // Test with different sized series (should throw)
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{1, 2, 3, 4};
    
    // EXPECT_THROW(df::chunk(2, serie1, serie2), std::runtime_error);
}

// Run the tests
RUN_TESTS()
