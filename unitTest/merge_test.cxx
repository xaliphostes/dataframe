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
#include <dataframe/functional/utils/merge.h>

using ISerie = df::GenSerie<int>;

TEST(merge, basic) {
    ISerie s1(2, {1, 2, 3, 4});
    ISerie s2(2, {3, 4, 5, 6});
    ISerie s3(2, {7, 8, 9, 10});

    auto concatenated = df::utils::merge(s1, s2);
    auto interleaved = df::utils::merge(s1, s2, false);
    auto multi = df::utils::merge(true, s1, s2, s3);

    EXPECT_ARRAY_EQ(concatenated.asArray(),
                    df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6}).asArray());
    EXPECT_ARRAY_EQ(interleaved.asArray(),
                    df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6}).asArray());
    EXPECT_ARRAY_EQ(
        multi.asArray(),
        df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 9, 10}).asArray());
}

TEST(merge, make_merge) {
    ISerie s1(2, {1, 2, 3, 4});
    ISerie s2(2, {3, 4, 5, 6});
    ISerie s3(2, {7, 8, 9, 10});

    auto maker1 = df::utils::make_merge<int>(true);
    EXPECT_ARRAY_EQ(
        maker1(s1, s2, s3).asArray(),
        df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 9, 10}).asArray());

    auto maker2 = df::utils::make_merge<int>(false);
    EXPECT_ARRAY_EQ(
        maker2(s1, s2, s3).asArray(),
        df::Serie(2, {1, 2, 3, 4, 7, 8, 3, 4, 5, 6, 9, 10}).asArray());
}

RUN_TESTS()