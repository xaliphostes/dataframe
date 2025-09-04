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

#include "../TEST.h"
#include <dataframe/Dataframe.h>

TEST(Dataframe, basic) {
    df::Dataframe df;

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
    const auto &retrieved_ints = df.get<int>("integers");
    EXPECT_EQ(retrieved_ints.size(), 5);
    EXPECT_EQ(retrieved_ints[0], 1);

    const auto &retrieved_strs = df.get<std::string>("strings");
    EXPECT_EQ(retrieved_strs.size(), 3);
    EXPECT_EQ(retrieved_strs[0], "a");

    // Test type safety
    // EXPECT_THROW(df.get<double>("integers"), std::runtime_error);

    // Test names
    auto names = df.names();
    EXPECT_EQ(names.size(), 2);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "integers") !=
                names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "strings") !=
                names.end());

    // Test remove
    df.remove("integers");
    EXPECT_EQ(df.size(), 1);
    EXPECT_FALSE(df.has("integers"));

    // Test clear
    df.clear();
    EXPECT_EQ(df.size(), 0);
    EXPECT_FALSE(df.has("strings"));
}

TEST(Dataframe, stress3d) {
    df::Dataframe df;

    // Test with Stress3D type
    df::Serie<Stress3D> stress{{1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12}};

    EXPECT_NO_THROW(df.add("stress", stress));
    const auto &retrieved = df.get<Stress3D>("stress");
    EXPECT_EQ(retrieved.size(), 2);
    EXPECT_EQ(retrieved[0][0], 1);
    EXPECT_EQ(retrieved[1][5], 12);
}

TEST(Dataframe, duplicates) {
    df::Dataframe df;
    df::Serie<int> serie1{1, 2, 3};
    df::Serie<int> serie2{4, 5, 6};

    df.add("test", serie1);
    EXPECT_THROW(df.add("test", serie2), std::runtime_error);
}

TEST(dataframe, type_safety) {
    MSG("Testing Dataframe type safety");

    df::Dataframe df;

    // Add series of different types
    df::Serie<double> doubles = {1.0, 2.0, 3.0};
    df::Serie<Vector2> vectors = {{1.0, 2.0}, {3.0, 4.0}};
    df.add("doubles", doubles);
    df.add("vectors", vectors);

    // Test correct type access
    EXPECT_NO_THROW(df.get<double>("doubles"));
    EXPECT_NO_THROW(df.get<Vector2>("vectors"));

    // Test type mismatch
    EXPECT_THROW(df.get<Vector2>("doubles"), std::runtime_error);
    EXPECT_THROW(df.get<double>("vectors"), std::runtime_error);
    
    // Test type inspection
    EXPECT_TRUE(df.type("doubles") == std::type_index(typeid(df::Serie<double>)));
    EXPECT_TRUE(df.type("vectors") == std::type_index(typeid(df::Serie<Vector2>)));
    //EXPECT_STREQ(df.type_name("doubles").c_str(), "double");
    //EXPECT_STREQ(df.type_name("vectors").c_str(), "array<double, 2>");
}

TEST(dataframe, error_handling) {
    MSG("Testing Dataframe error handling");

    df::Dataframe df;
    df::Serie<double> values = {1.0, 2.0, 3.0};

    // Test duplicate addition
    df.add("test", values);
    EXPECT_THROW(df.add("test", values), std::runtime_error);

    // Test non-existent series
    EXPECT_THROW(df.get<double>("nonexistent"), std::runtime_error);
    EXPECT_THROW(df.type("nonexistent"), std::runtime_error);

    // Test removal
    EXPECT_NO_THROW(df.remove("test"));
    EXPECT_THROW(df.remove("test"), std::runtime_error); // Already removed
}

TEST(dataframe, basic_operations) {
    MSG("Testing Dataframe basic operations");

    df::Dataframe df;
    EXPECT_EQ(df.size(), 0);
    EXPECT_TRUE(df.names().empty());

    // Add some series
    df::Serie<double> s1 = {1.0, 2.0};
    df::Serie<Vector2> s2 = {{1.0, 1.0}, {2.0, 2.0}};
    df.add("s1", s1);
    df.add("s2", s2);

    // Check size and names
    EXPECT_EQ(df.size(), 2);
    auto names = df.names();
    EXPECT_EQ(names.size(), 2);
    EXPECT_TRUE(std::find(names.begin(), names.end(), "s1") != names.end());
    EXPECT_TRUE(std::find(names.begin(), names.end(), "s2") != names.end());

    // Test clear
    df.clear();
    EXPECT_EQ(df.size(), 0);
    EXPECT_TRUE(df.names().empty());
}

RUN_TESTS();