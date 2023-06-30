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

#include <iostream>
#include "../src/Serie.h"
#include "../src/utils.h"
#include "../src/algos/reduce.h"
#include "assertions.h"

int main()
{
    df::Serie serie(3, {1,2,3, 4,5,6}); // 2 items with itemSize = 3

    df::Serie s = df::reduce(
        serie, [](const Array &prev, const Array &cur) {
            return Array{prev[0] + cur[0], prev[1] + cur[1], prev[2] + cur[2]};
        }, Array{10,20,30}
    );

    Array sol{10 + 1 + 4, 20 + 2 + 5,30 + 3 + 6};
    assertArrayEqual(s.asArray(), sol);

    return 0;
}
