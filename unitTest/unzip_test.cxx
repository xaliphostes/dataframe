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
#include <dataframe/functional/utils/unzip.h>
#include <dataframe/functional/utils/zip.h>

TEST(unzip, basic) {
    // Original Series
    df::Serie s1(1, {1, 2});                // Scalar
    df::Serie s2(2, {3, 4, 5, 6});          // 2D
    df::Serie s3(3, {7, 8, 9, 10, 11, 12}); // 3D

    // Zip them
    auto zipped = df::utils::zip(s1, s2, s3);
    assertSerieEqual(zipped, Array{1, 3, 4, 7, 8, 9, 2, 5, 6, 10, 11, 12});

    // Unzip them
    auto series = df::utils::unzip(zipped, {1, 2, 3});

    // Check results
    assertEqual<uint>(series.size(), 3);
    assertSerieEqual(series[0], s1);
    assertSerieEqual(series[1], s2);
    assertSerieEqual(series[2], s3);
}

RUN_TESTS()