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
#include <dataframe/functional/utils/sort.h>

using namespace df;

TEST(sort, basic) {
    Serie s1(1, {3, 1, 4, 1, 5});
    assertSerieEqual(utils::sort(s1), {1, 1, 3, 4, 5});

    Serie s2(2, {3, 1, 1, 2, 2, 2});
    assertSerieEqual(utils::sort(s2), {1, 2, 2, 2, 3, 1}); // Lexicographic sort

    // Custom comparator
    auto sorted3 = utils::sort(
        s2, [](const Array &a, const Array &b) { return a[1] < b[1]; });
    assertSerieEqual(sorted3, {3, 1, 1, 2, 2, 2});
}

RUN_TESTS()