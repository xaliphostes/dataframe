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
#include <dataframe/math/add.h>
#include <dataframe/math/sub.h>

using namespace df;


TEST(sub, op) {
    MSG("Testing series subtraction");

    // Test integer subtraction
    Serie<int> s1{10, 20, 30, 40};
    Serie<int> s2{1, 2, 3, 4};
    auto result = sub(s1, s2);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<int>({9, 18, 27, 36}));

    // Test float subtraction
    Serie<float> f1{3.5f, 4.5f, 5.5f};
    Serie<float> f2{0.5f, 1.5f, 2.5f};
    auto float_result = sub(f1, f2);
    EXPECT_ARRAY_NEAR(float_result.asArray(),
                      std::vector<float>({3.0f, 3.0f, 3.0f}), 1e-6f);

    // Test mixed type subtraction
    Serie<int> i1{5, 6, 7};
    Serie<double> d1{0.5, 1.5, 2.5};
    auto mixed_result = sub(i1, d1);
    EXPECT_ARRAY_NEAR(mixed_result.asArray(),
                      std::vector<double>({4.5, 4.5, 4.5}), 1e-6);

    // Test error on size mismatch
    Serie<int> s3{1, 2};
    EXPECT_THROW(sub(s1, s3), std::runtime_error);

    // Test pipeline operation
    auto result_pipe = s1 | bind_sub(s2);
    EXPECT_ARRAY_EQ(result_pipe.asArray(), std::vector<int>({9, 18, 27, 36}));
}

RUN_TESTS()