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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "../TEST.h"
#include <dataframe/math/equals.h>

using namespace df;

TEST(Serie, EqualsInteger) {
    MSG("Testing equals with integer Series");

    Serie<int> s1{1, 2, 3};
    Serie<int> s2{1, 2, 3};
    Serie<int> s3{1, 2, 4};
    Serie<int> s4{1, 2};

    EXPECT_TRUE(equals(s1, s2));
    EXPECT_FALSE(equals(s1, s3));
    EXPECT_FALSE(equals(s1, s4));

    // Test pipeline operation
    EXPECT_TRUE(s1 | bind_equals(s2));
    EXPECT_FALSE(s1 | bind_equals(s3));
}

TEST(Serie, EqualsDouble) {
    MSG("Testing equals with double Series and tolerance");

    Serie<double> s1{1.0, 2.0, 3.0};
    Serie<double> s2{1.0, 2.0, 3.0};
    Serie<double> s3{1.0, 2.0, 3.0 + 1e-13};  // Within default tolerance
    Serie<double> s4{1.0, 2.0, 3.1};          // Outside default tolerance

    EXPECT_TRUE(equals(s1, s2));
    EXPECT_TRUE(equals(s1, s3));  // Should pass with default tolerance
    EXPECT_FALSE(equals(s1, s4));

    // Test with custom tolerance
    EXPECT_FALSE(equals(s1, s3, 1e-14));  // Should fail with tighter tolerance
    EXPECT_TRUE(equals(s1, s4, 0.2));     // Should pass with looser tolerance

    // Test pipeline operation
    EXPECT_TRUE(s1 | bind_equals(s2));
    EXPECT_TRUE(s1 | bind_equals(s3));
    EXPECT_FALSE(s1 | bind_equals(s4));
    EXPECT_TRUE(s1 | bind_equals(s4, 0.2));
}

TEST(Serie, EqualsFloat) {
    MSG("Testing equals with float Series and tolerance");

    Serie<float> s1{1.0f, 2.0f, 3.0f};
    Serie<float> s2{1.0f, 2.0f, 3.0f};
    Serie<float> s3{1.0f, 2.0f, 3.0f + 1e-6f};  // Within default tolerance
    Serie<float> s4{1.0f, 2.0f, 3.1f};          // Outside default tolerance

    EXPECT_TRUE(equals(s1, s2));
    EXPECT_TRUE(equals(s1, s3));  // Should pass with default tolerance
    EXPECT_FALSE(equals(s1, s4));

    // Test with custom tolerance
    EXPECT_FALSE(equals(s1, s3, 1e-8f));  // Should fail with tighter tolerance
    EXPECT_TRUE(equals(s1, s4, 0.2f));    // Should pass with looser tolerance

    // Test pipeline operation
    EXPECT_TRUE(s1 | bind_equals(s2));
    EXPECT_TRUE(s1 | bind_equals(s3));
    EXPECT_FALSE(s1 | bind_equals(s4));
    EXPECT_TRUE(s1 | bind_equals(s4, 0.2f));
}

RUN_TESTS()