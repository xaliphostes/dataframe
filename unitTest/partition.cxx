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
#include <dataframe/functional/partition.h>
#include <dataframe/functional/print.h>

START_TEST(partition) {
    df::Serie stress(6,
                     {2, 4, 6, 3, 6, 9, -1, 2, 3, 4, 5, 6, -9, 8, 7, 6, 5, 4});

    auto [compressive, tensile] = partition(
        [](const Array &s, uint32_t) {
            return s[0] < 0; // Separate compressive from tensile states (xx
                             // component of stress)
        },
        stress);

    assertSerieEqual(compressive,
                     df::Serie(6, {-1, 2, 3, 4, 5, 6, -9, 8, 7, 6, 5, 4}));
    assertSerieEqual(tensile, df::Serie(6, {2, 4, 6, 3, 6, 9}));
}
END_TEST()

int main() {

    partition();

    return 0;
}
