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
#include <dataframe/utils/sort.h>

TEST(sort, basic_sort_ascending) {
    df::Serie<int> serie{3, 1, 4, 1, 5, 2};

    auto result = df::sort(serie);
    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(result[0] == 1);
    EXPECT_TRUE(result[1] == 1);
    EXPECT_TRUE(result[5] == 5);

    // Check if original serie is unchanged
    EXPECT_TRUE(serie[0] == 3);
}

TEST(sort, sort_descending) {
    df::Serie<double> serie{3.2, 1.1, 4.3, 1.0, 5.5};

    auto result = df::sort(serie, false);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 5.5);
    EXPECT_TRUE(result[4] == 1.0);
}

TEST(sort, custom_comparator) {
    df::Serie<int> serie{-3, 1, -4, 2, -1, 5};

    auto result = df::sort(serie, [](const int &a, const int &b) {
        return std::abs(a) < std::abs(b);
    });
    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(std::abs(result[0]) == 1);
    EXPECT_TRUE(std::abs(result[5]) == 5);
}

TEST(sort, bind_sort_usage) {
    df::Serie<int> serie{3, 1, 4, 1, 5};

    auto bound = df::bind_sort(false); // descending
    auto result = bound(serie);
    EXPECT_TRUE(result.size() == 5);
    EXPECT_TRUE(result[0] == 5);
    EXPECT_TRUE(result[4] == 1);
}

TEST(sort, empty_serie) {
    df::Serie<int> serie;

    auto result = df::sort(serie);
    EXPECT_TRUE(result.size() == 0);
}

TEST(sort, single_element) {
    df::Serie<int> serie{42};

    auto result = df::sort(serie);
    EXPECT_TRUE(result.size() == 1);
    EXPECT_TRUE(result[0] == 42);
}

RUN_TESTS();