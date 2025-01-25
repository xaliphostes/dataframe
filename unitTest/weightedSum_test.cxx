
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
#include <dataframe/functional/math/weightedSum.h>

TEST(weightedSum, basic1) {
    df::GenSerie<double> a(2, {1, 2, 3, 4});
    df::GenSerie<double> b(2, {4, 3, 2, 1});
    df::GenSerie<double> c(2, {2, 2, 1, 1});

    auto res = df::math::weigthedSum({a, b, c}, {2.0, 3.0, 4.0});

    // Expected values: 2*(1,2) + 3*(4,3) + 4*(2,2) = (2,4) + (12,9) + (8,8) =
    // (22,21)
    //                 2*(3,4) + 3*(2,1) + 4*(1,1) = (6,8) + (6,3) + (4,4) =
    //                 (16,15)
    EXPECT_EQ(res.itemSize(), 2);
    EXPECT_EQ(res.count(), 2);
    EXPECT_ARRAY_EQ(res.asArray(), std::vector<double>({22, 21, 16, 15}));
}

TEST(weightedSum, basic2) {
    df::Serie a(2, {1, 2, 3, 4});
    df::Serie b(2, {4, 3, 2, 1});
    df::Serie c(2, {2, 2, 1, 1});
    df::Serie d(3, {2, 2, 1, 1, 0, 0});
    df::Serie e(2, {2, 2, 1, 1, 0, 0});

    Array<double> weights{2, 3, 4};
    {
        df::Serie s = df::math::weigthedSum({a, b, c}, weights);
        EXPECT_ARRAY_EQ(s.asArray(), Array<double>({22, 21, 16, 15}));
    }

    {
        auto s = df::math::weigthedSum({a, b, c}, {2, 3, 4});
        EXPECT_ARRAY_EQ(s.asArray(), Array<double>({22, 21, 16, 15}));
    }

    EXPECT_THROW(df::math::weigthedSum({a, b}, weights), std::invalid_argument);
    EXPECT_THROW(df::math::weigthedSum({a, b, d}, weights),
                 std::invalid_argument);
    EXPECT_THROW(df::math::weigthedSum({a, b, e}, weights),
                 std::invalid_argument);

    {
        df::Serie a(2, {1, 2, 3, 4});
        df::Serie b(2, {4, 3, 2, 1});
        df::Serie c(2, {2, 2, 1, 1});

        // Performs s = 2*a + 3*b + 4*c
        auto s = df::math::weigthedSum({a, b, c}, {2, 3, 4});
        df::print(s);
    }
}

TEST(weightedSum, error_handling) {
    df::GenSerie<double> a(2, {1, 2});
    df::GenSerie<double> b(2, {3, 4});
    df::GenSerie<double> mismatched(3, {1, 2, 3});

    // Test mismatched weights
    EXPECT_THROW(df::math::weigthedSum({a, b}, {1.0}), std::invalid_argument);

    // Test mismatched itemSize
    EXPECT_THROW(df::math::weigthedSum({a, mismatched}, {1.0, 1.0}),
                 std::invalid_argument);

    // // Test empty list
    // auto empty = df::math::weigthedSum({}, {});
    // EXPECT_TRUE(empty.isEmpty());
}

RUN_TESTS()