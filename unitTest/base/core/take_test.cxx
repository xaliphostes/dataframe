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
#include <dataframe/core/take.h>
#include <string>

TEST(Utils, TakeSingleSerie) {
    // Create a test series
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Test taking first 5 elements
    auto first_five = df::take(numbers, 5);
    
    EXPECT_EQ(first_five.size(), 5);
    EXPECT_ARRAY_EQ(first_five.asArray(), std::vector<int>({1, 2, 3, 4, 5}));
    
    // Test taking more elements than the series contains
    auto all_plus = df::take(numbers, 15);
    
    EXPECT_EQ(all_plus.size(), 10);
    EXPECT_ARRAY_EQ(all_plus.asArray(), numbers.asArray());
    
    // Test taking zero elements
    auto empty = df::take(numbers, 0);
    
    EXPECT_EQ(empty.size(), 0);
    EXPECT_TRUE(empty.empty());
}

TEST(Utils, TakeMultipleSeries) {
    df::Serie<int> numbers{1, 2, 3, 4, 5};
    df::Serie<std::string> names{"Alice", "Bob", "Charlie", "Dave", "Eve"};
    df::Serie<double> values{10.1, 20.2, 30.3, 40.4, 50.5};
    
    // Test taking first 3 elements from all series
    auto [num, nam, val] = df::take(3, numbers, names, values);
    
    EXPECT_EQ(num.size(), 3);
    EXPECT_EQ(nam.size(), 3);
    EXPECT_EQ(val.size(), 3);
    
    EXPECT_ARRAY_EQ(num.asArray(), std::vector<int>({1, 2, 3}));
    EXPECT_STREQ(nam[0].c_str(), "Alice");
    EXPECT_STREQ(nam[1].c_str(), "Bob");
    EXPECT_STREQ(nam[2].c_str(), "Charlie");
    EXPECT_ARRAY_EQ(val.asArray(), std::vector<double>({10.1, 20.2, 30.3}));
    
    // Test taking all elements
    auto [all_num, all_nam, all_val] = df::take(5, numbers, names, values);
    
    EXPECT_EQ(all_num.size(), 5);
    EXPECT_ARRAY_EQ(all_num.asArray(), numbers.asArray());
}

TEST(Utils, TakeSkipWithPipe) {
    df::Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Test bind_take
    auto first_four = numbers | df::bind_take<int>(4);
    
    EXPECT_EQ(first_four.size(), 4);
    EXPECT_ARRAY_EQ(first_four.asArray(), std::vector<int>({1, 2, 3, 4}));
            
}

TEST(Utils, TakeEdgeCases) {
    // Test with empty series
    df::Serie<int> empty_serie;
    
    auto take_empty = df::take(empty_serie, 5);
    EXPECT_EQ(take_empty.size(), 0);
        
    // Test with different sized series (should throw)
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{1, 2, 3, 4};
    
    EXPECT_THROW(df::take(2, serie1, serie2), std::runtime_error);
}

// Run the tests
RUN_TESTS()
