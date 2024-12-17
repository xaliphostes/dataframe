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
#include <dataframe/functional/conditional/check.h>
#include <dataframe/functional/math/normalize.h>
#include <iostream>

using namespace df;

TEST(check, _1) {
    Serie s1(1, {1, -2, 3});
    auto isNegative =
        df::cond::check(s1, [](double v, uint32_t) { return v < 0; });
    assertSerieEqual(isNegative, Array{0, 1, 0});

    // Pour une Serie vectorielle
    Serie s2(3, {1, 2, 3, -4, 5, 6});
    auto firstIsNegative =
        df::cond::check(s2, [](const Array &v, uint32_t) { return v[0] < 0; });
    assertSerieEqual(firstIsNegative, Array{0, 1});

    auto checkFirstNegative = df::cond::make_check([](const auto &v, uint32_t) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v < 0;
        } else {
            return v[0] < 0;
        }
    });
}

RUN_TESTS()
