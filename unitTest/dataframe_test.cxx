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

#include "TEST.h"
#include <dataframe/Dataframe.h>

TEST(DataFrame, basic) {
    df::DataFrame df;
    
    // Test adding series
    df::Serie<int> ints{1, 2, 3, 4, 5};
    df::Serie<std::string> strs{"a", "b", "c"};
    
    EXPECT_NO_THROW(df.add("integers", ints));
    EXPECT_NO_THROW(df.add("strings", strs));
    
    // Test size and has
    EXPECT_EQ(df.size(), 2);
    EXPECT_TRUE(df.has("integers"));
    EXPECT_TRUE(df.has("strings"));
    EXPECT_FALSE(df.has("nonexistent"));
    
    // Test retrieval
    const auto& retrieved_ints = df.get<int>("integers");
    EXPECT_EQ(retrieved_ints.size(), 5);
    EXPECT_EQ(retrieved_ints[0], 1);
    
    const auto& retrieved_strs = df.get<std::string>("strings");
    EXPECT_EQ(retrieved_strs.size(), 3);
    EXPECT_EQ(retrieved_strs[0], "a");
    
    // Test type safety
    // EXPECT_THROW(df.get<double>("integers"), std::runtime_error);
    
    // Test names
    auto names = df.names();
    EXPECT_EQ(names.size(), 2);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "integers") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "strings") != names.end());
    
    // Test remove
    df.remove("integers");
    EXPECT_EQ(df.size(), 1);
    EXPECT_FALSE(df.has("integers"));
    
    // Test clear
    df.clear();
    EXPECT_EQ(df.size(), 0);
    EXPECT_FALSE(df.has("strings"));
}

TEST(DataFrame, stress3d) {
    df::DataFrame df;
    
    // Test with Stress3D type
    df::Serie<Stress3D> stress{
        {1, 2, 3, 4, 5, 6},
        {7, 8, 9, 10, 11, 12}
    };
    
    EXPECT_NO_THROW(df.add("stress", stress));
    const auto& retrieved = df.get<Stress3D>("stress");
    EXPECT_EQ(retrieved.size(), 2);
    EXPECT_EQ(retrieved[0][0], 1);
    EXPECT_EQ(retrieved[1][5], 12);
}

TEST(DataFrame, duplicates) {
    df::DataFrame df;
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{4, 5, 6};
    
    df.add("test", serie1);
    EXPECT_THROW(df.add("test", serie2), std::runtime_error);
}

RUN_TESTS();