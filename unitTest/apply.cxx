/*
 * Copyright (c) 2023 fmaerten@gmail.com
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

#include "assertions.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/functional/utils/apply.h>
#include <iostream>

START_TEST(apply1) {
    df::Serie a(1, {1, 2, 3, 4});

    auto s = df::utils::apply([](const Array &a, uint32_t i) {
        Array r = a;
        for (auto &v : r) {
            v = std::sqrt(v);
        }
        return r;
    }, a);

    assertArrayEqual(s.asArray(), Array{1, std::sqrt(2), std::sqrt(3), 2});
}
END_TEST()

START_TEST(apply2) {
    df::Serie a(1, {1, 2, 3, 4});

    auto sqrt = df::utils::make_apply([](const Array &a, uint32_t i) {
        Array r = a;
        for (auto &v : r) {
            v = std::sqrt(v);
        }
        return r;
    });

    auto s = sqrt(a);
    assertArrayEqual(s.asArray(), Array{1, std::sqrt(2), std::sqrt(3), 2});
}
END_TEST()

int main() {
    apply1();
    apply2();

    return 0;
}
