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
#include <dataframe/operations/math/div.h>
#include "assertions.h"

int main()
{
    std::vector<Array> sol {{1, 2, 3}, {1, 2, 3}};
    
    df::Serie a(3, {2, 4, 6, 3, 6, 9});
    df::Serie divider(1, {2, 3});
    df::Serie r = df::div(a, divider);

    for (uint32_t i = 0; i < r.count(); ++i)
    {
        assertArrayEqual(r.value(i), sol[i]);
    }
 
    divider = df::Serie(2, {1, 3, 2, 9});
    shouldThrowError([a, divider](){
        df::div(a, divider);
    });

    divider = df::Serie(1, {1, 3, 2});
    shouldThrowError([a, divider](){
        df::div(a, divider);
    });

    return 0;
}
