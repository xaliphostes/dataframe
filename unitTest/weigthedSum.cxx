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
#include <dataframe/Serie.h>
#include <dataframe/operations/weightedSum.h>
#include "assertions.h"

int main()
{
    df::Serie a(2, {1, 2, 3, 4});
    df::Serie b(2, {4, 3, 2, 1});
    df::Serie c(2, {2, 2, 1, 1});
    df::Serie d(3, {2, 2, 1, 1, 0, 0});
    df::Serie e(2, {2, 2, 1, 1, 0, 0});

    Array weights{2, 3, 4};
    {
        df::Serie s = df::weigthedSum({a, b, c}, weights);
        assertArrayEqual(s.asArray(), Array{22,21,16,15});
    }

    {
        auto s = df::weigthedSum({a, b, c}, {2, 3, 4});
        assertArrayEqual(s.asArray(), Array{22,21,16,15});
    }

    shouldThrowError([a, b, weights]() {
        auto s = df::weigthedSum({a, b}, weights);
    });

    shouldThrowError([a, b, d, weights]() {
        df::weigthedSum({a, b, d}, weights);
    });

    shouldThrowError([a, b, e, weights]() {
        df::weigthedSum({a, b, e}, weights);
    });

    return 0;
}
