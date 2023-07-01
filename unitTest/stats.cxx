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
#include <dataframe/Serie.h>
#include <dataframe/operations/stats/bins.h>
#include "assertions.h"

void bins()
{
    df::Serie s(1, {57, 57, 57, 58, 63, 66, 66, 67, 67, 68, 69, 70, 70, 70, 70, 72, 73, 75, 75, 76, 76, 78, 79, 81});

    {
        df::Serie a = df::bins(s, 10);

        uint32_t nb1 = s.count();
        Array sol{4, 0, 1, 2, 3, 5, 2, 4, 1, 2};
        assertArrayEqual(a.asArray(), sol);

        auto nb2 = std::reduce(sol.begin(), sol.end());
        assertEqual(nb1, (uint32_t)nb2);
    }

    {
        df::Serie a = df::bins(s, 10, 0, 100);
        uint nb1 = s.count();
        Array sol{0, 0, 0, 0, 0, 4, 7, 12, 1, 0};
        assertArrayEqual(a.asArray(), sol);

        auto nb2 = std::reduce(sol.begin(), sol.end());
        assertEqual(nb1, (uint32_t)nb2);
    }
}

int main()
{
    bins();

    return 0;
}
