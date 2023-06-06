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
#include <cmath>
#include "../src/Serie.h"
#include "../src/math/transpose.h"
#include "assertions.h"

void m2() {
    /*
        1, 2
        3, 4
    */
    df::Serie a(4, {1, 2, 3, 4});

    /*
        1, 3,
        2, 4
    */
    auto s = df::transpose(a);
    assertArrayEqual(s, Array{1, 3, 2, 4});

    a = df::Serie(5, {1, 2, 3, 4, 5});
    shouldThrowError([a]() {
        auto s = df::transpose(a);
    });
}

void m3() {
    /*
        1, 2, 3
        4, 5, 6
        7, 8, 9
    */
    df::Serie a(9, {1, 2, 3, 4, 5, 6, 7, 8, 9});

    /*
        1, 4, 7
        2, 5, 8
        3, 6, 9
    */
    auto s = df::transpose(a);
    assertArrayEqual(s, Array{1, 4, 7, 2, 5, 8, 3, 6, 9});

    a = df::Serie(8, {1, 2, 3, 4, 5, 6, 7, 8});
    shouldThrowError([a]() {
        auto s = df::transpose(a);
    });
}

int main()
{
    
    m2();
    m3();
    
    return 0;
}
