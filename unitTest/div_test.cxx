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

#include "TEST.h"
#include <dataframe/functional/math/div.h>

TEST(div, test1) {
    // Division by a scalar
    {
        df::GenSerie<double> s(1, {2, 4, 6, 8, 10, 12});
        auto sol = Array<double>({1, 2, 3, 4, 5, 6});
        EXPECT_ARRAY_EQ(df::math::div(s, 2.0).asArray(), sol);
        EXPECT_ARRAY_EQ((s / 2.0).asArray(), sol);
    }

    // Division scalar series by scalar series
    {
        df::GenSerie<double> a1(1, {6, 8, 10});
        df::GenSerie<double> a2(1, {2, 4, 5});
        auto sol = Array<double>({3, 2, 2});
        auto r1 = df::math::div(a1, a2); // Should give {3, 2, 2}
        auto r2 = a1 / a2;
        EXPECT_ARRAY_EQ(r1.asArray(), sol);
        EXPECT_ARRAY_EQ(r2.asArray(), sol);
    }

    // Division of vector series by scalar
    {
        df::GenSerie<double> s2(3, {2, 4, 6, 8, 10, 12}); // 2 vectors of size 3
        auto sol = Array<double>({1, 2, 3, 4, 5, 6});
        EXPECT_ARRAY_EQ(df::math::div(s2, 2.0).asArray(), sol);
        EXPECT_ARRAY_EQ((s2 / 2.0).asArray(), sol);
    }

    // Division of vector series by scalar series
    {
        df::GenSerie<double> v1(3, {2, 4, 6, 8, 10, 12}); // 2 vectors of size 3
        df::GenSerie<double> v2(1, {2, 4}); // One scalar per vector
        auto sol = Array<double>({1, 2, 3, 2, 2.5, 3});
        auto r1 = df::math::div(v1, v2);    // Should give {1,2,3, 2,2.5,3}
        auto r2 = v1 / v2;
        EXPECT_ARRAY_EQ(r1.asArray(), sol);
        EXPECT_ARRAY_EQ(r2.asArray(), sol);
    }

    // Division of vector series by vector series
    {
        df::GenSerie<double> v3(3, {2, 4, 6, 8, 10, 12});
        df::GenSerie<double> v4(3, {2, 2, 2, 4, 4, 4}); // Element-wise division
        auto sol = Array<double>({1, 2, 3, 2, 2.5, 3});
        auto r1 = df::math::div(v3, v4); // Should give {1,2,3, 2,2.5,3}
        auto r2 = v3 / v4;
        df::print(r1);
        df::print(r2);
        // EXPECT_ARRAY_EQ(r1.asArray(), sol);
        // EXPECT_ARRAY_EQ(r2.asArray(), sol);
    }

    // Test division by zero handling
    {
        df::GenSerie<double> z1(1, {1, 2, 3});
        df::GenSerie<double> z2(1, {0, 2, 0});
        auto r6 = df::math::div(
            z1, z2); // Should handle division by zero appropriately
        df::print(r6);
    }
}

// Test error cases
// TEST(div, errors) {
//     // Test mismatched counts
//     df::GenSerie<double> e1(1, {1, 2, 3});
//     df::GenSerie<double> e2(1, {1, 2});
//     EXPECT_THROW(df::math::div(e1, e2), std::invalid_argument);

//     // Test mismatched itemSize
//     df::GenSerie<double> e3(3, {1, 2, 3, 4, 5, 6});
//     df::GenSerie<double> e4(2, {1, 2, 3, 4});
//     EXPECT_THROW(df::math::div(e3, e4), std::invalid_argument);

//     // Test scalar divided by vector
//     df::GenSerie<double> e5(1, {1, 2, 3});
//     df::GenSerie<double> e6(3, {1, 2, 3, 4, 5, 6});
//     EXPECT_THROW(df::math::div(e5, e6), std::invalid_argument);
// }

RUN_TESTS()