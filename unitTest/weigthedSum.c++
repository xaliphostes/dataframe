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
#include "../src/Dataframe.h"
#include "../src/utils.h"
#include "../src/math/weightedSum.h"
#include "../src/math/add.h"
#include "../src/math/dot.h"
#include "../src/math/negate.h"

void assertEqual(double a, double b) {
    if (a != b) {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b << std::endl ;
        exit(-1);
    }
}

int main()
{
    df::Serie a(2, {1, 2, 3, 4});
    df::Serie b(2, {4, 3, 2, 1});
    df::Serie c(2, {2, 2, 1, 1});
    df::Serie d(3, {2, 2, 1, 1, 0, 0});
    df::Serie e(2, {2, 2, 1, 1, 0, 0});

    Array weights{2, 3, 4};
    {
        auto s = df::weigthedSum({a, b, c}, weights).asArray();
        assertEqual(s[0], 22);
        assertEqual(s[1], 21);
        assertEqual(s[2], 16);
        assertEqual(s[3], 15);
    }

    {
        auto s = df::weigthedSum({a, b, c}, {2, 3, 4}).asArray();
        assertEqual(s[0], 22);
        assertEqual(s[1], 21);
        assertEqual(s[2], 16);
        assertEqual(s[3], 15);
    }
    
    try
    {
        auto s = df::weigthedSum({a, b}, weights);
        exit(-1);
    }
    catch (std::invalid_argument &e)
    {
        // Ok throw an error
        df::error(e.what());
    }

    try
    {
        df::weigthedSum({a, b, d}, weights);
        exit(-1);
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
    }

    try
    {
        df::weigthedSum({a, b, e}, weights);
        exit(-1);
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
    }

    return 0;
}
