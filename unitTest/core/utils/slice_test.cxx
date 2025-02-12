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

#include "../../TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/utils/slice.h>

TEST(slice, basic_slice) {
    df::Serie<int> serie{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto result = df::slice(serie, 2, 5); // [2, 3, 4]
    EXPECT_TRUE(result.size() == 3);
    EXPECT_TRUE(result[0] == 2);
    EXPECT_TRUE(result[2] == 4);
}

TEST(slice, slice_start_zero) {
    df::Serie<int> serie{0, 1, 2, 3, 4};

    auto result = df::slice(serie, 3); // [0, 1, 2]
    EXPECT_TRUE(result.size() == 3);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[2] == 2);
}

TEST(slice, slice_with_step) {
    df::Serie<int> serie{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto result = df::slice(serie, 0, 6, 2); // [0, 2, 4]
    EXPECT_TRUE(result.size() == 3);
    EXPECT_TRUE(result[0] == 0);
    EXPECT_TRUE(result[1] == 2);
    EXPECT_TRUE(result[2] == 4);
}

TEST(slice, bind_slice_usage) {
    df::Serie<int> serie{0, 1, 2, 3, 4, 5};

    auto bound = df::bind_slice(2, 4);
    auto result = bound(serie); // [2, 3]
    EXPECT_TRUE(result.size() == 2);
    EXPECT_TRUE(result[0] == 2);
    EXPECT_TRUE(result[1] == 3);
}

TEST(slice, empty_result) {
    df::Serie<int> serie{0, 1, 2, 3, 4};

    auto result = df::slice(serie, 2, 2); // []
    EXPECT_TRUE(result.size() == 0);
}

TEST(slice, error_handling) {
    df::Serie<int> serie{0, 1, 2, 3, 4};

    try {
        auto result = df::slice(serie, 3, 2); // Should throw
        EXPECT_TRUE(false);                        // Should not reach here
    } catch (const std::invalid_argument &) {
        EXPECT_TRUE(true);
    }

    try {
        auto result = df::slice(serie, 0, 6); // Should throw
        EXPECT_TRUE(false);                        // Should not reach here
    } catch (const std::out_of_range &) {
        EXPECT_TRUE(true);
    }
}

RUN_TESTS();