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
#include <dataframe/Serie.h>
#include <dataframe/forEach.h>
#include <dataframe/map.h>
#include <dataframe/reduce.h>

TEST(reduce, double) {
    auto numbers = df::Serie<double>{1, 2, 3, 4, 5};

    auto sum = df::reduce(
        [](double acc, double value, double index) { return acc + value; },
        numbers, 0); // 15

    std::cerr << "sum = " << sum << std::endl;
}

TEST(reduce, vector2) {
    auto numbers =
        df::Serie<Vector2>{Vector2{1, 2}, Vector2{3, 4}, Vector2{5, 6}};

    auto sum1 = df::reduce(
        [](const Vector2 &acc, const Vector2 &v, size_t) {
            return Vector2{acc[0] + v[0], acc[1] + v[1]};
        },
        numbers, Vector2{0, 0}); // [9, 12]

    // df::print(sum1);

    // ---------------------

    auto sum2 = df::reduce(
        [](const Vector3 &acc, const Vector2 &v, size_t index) {
            return Vector3{acc[0] + v[0], acc[1] + v[1],
                           double(acc[2] + index)};
        },
        numbers, Vector3{0, 0, 0}); // [9, 12, 3]

    df::print(sum2);
}

TEST(reduce, 2_series) {
    auto number1 = df::Serie<double>{1, 2, 3};
    auto number2 = df::Serie<double>{4, 5, 6};

    auto sum =
        df::reduce([](const Vector2 &acc, double v1, double v2,
                      size_t) { return Vector2{acc[0] + v1, acc[1] + v2}; },
                   number1, number2, Vector2{0, 0}); // [6, 15]

    df::print(sum);
}

RUN_TESTS();