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
#include <dataframe/Serie.h>
#include <dataframe/functional/utils/print.h>
#include <dataframe/functional/utils/reject.h>
#include <iostream>

int main() {

    // Single Serie reject
    df::Serie s1(1, {1, 2, 3, 4, 5});
    auto noEvens =
        df::utils::reject([](double v, uint32_t) { return (int)v % 2 == 0; },
                          s1); // Keep odd numbers
    df::utils::print(noEvens);

    // Multiple Series reject
    df::Serie stress(6, {1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1});
    df::Serie positions(3, {1, 2, 3, 3, 2, 1});
    auto rejected = df::utils::rejectAll(
        [](const Array &s, const Array &p) {
            return s[0] < 0 &&
                   p[2] > 0; // Remove where both conditions are true
        },
        stress, positions);

    df::utils::print(rejected[0]);
    df::utils::print(rejected[1]);

    // Using make_reject
    auto removeNegatives =
        df::utils::make_reject([](double v, uint32_t) { return v < 0; });
    auto positives = removeNegatives(s1);
    df::utils::print(positives);

    return 0;
}
