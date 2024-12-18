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
#include <dataframe/functional/utils/zip.h>

TEST(zip, basic) {
    // Create some test Series
    df::Serie s1(1, {1, 2, 3});          // scalar serie
    df::Serie s2(2, {4, 5, 6, 7, 8, 9}); // 2D serie
    df::Serie s3(1, {10, 11, 12});       // scalar serie

    // Using variadic zip
    auto result1 = df::utils::zip(s1, s2, s3);
    assertEqual<uint>(result1.count(), 3);
    assertEqual<uint>(result1.itemSize(), 4);
    assertArrayEqual(result1.asArray(), {1, 4, 5, 10, 2, 6, 7, 11, 3, 8, 9, 12});
    // std::cerr << result1 << std::endl;

    // Using vector zip
    std::vector<df::Serie> series = {s1, s2, s3};
    auto result2 = df::utils::zipVector(series);
    assertEqual<uint>(result2.count(), 3);
    assertEqual<uint>(result2.itemSize(), 4);
    assertArrayEqual(result1.asArray(), {1, 4, 5, 10, 2, 6, 7, 11, 3, 8, 9, 12});

    // The results can be used in further operations
    result1.forEach([](const Array &values, uint32_t i) {
        // Process the combined values
        // values will contain {1,4,5,10} for i=0, {2,6,7,11} for i=1, etc.
    });
}

RUN_TESTS()