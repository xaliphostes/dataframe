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
#include <dataframe/functional/merge.h>
#include <dataframe/functional/print.h>

START_TEST(merge) {
    df::Serie s1(2, {1, 2, 3, 4});
    df::Serie s2(2, {3, 4, 5, 6});
    df::Serie s3(2, {7, 8, 9, 10});

    auto concatenated = df::merge(s1, s2);
    auto interleaved = df::merge(s1, s2, false);
    auto multi = merge(true, s1, s2, s3);

    assertSerieEqual(concatenated, df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6}));
    assertSerieEqual(interleaved, df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6}));
    assertSerieEqual(multi,
                     df::Serie(2, {1, 2, 3, 4, 3, 4, 5, 6, 7, 8, 9, 10}));
}
END_TEST()

int main() {

    merge();

    return 0;
}
