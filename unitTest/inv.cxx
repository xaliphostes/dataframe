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
#include <dataframe/functional/algebra/inv.h>
#include "assertions.h"

int main()
{
    {
        std::cerr << "TO CHECK !!!" << std::endl ;
        df::Serie s(6, {1, 2, 3, 4, 5, 6});
        auto result = df::inv(s);
        assertSerieEqual(result, {
            1, -3, 2,
                3, -1,
                    0
        });
    }

    {
        std::cerr << "TO CHECK !!!" << std::endl ;
        df::Serie s(9, {2, 3, 1, 6, 5, 4, 7, 9, 8});
        auto result = df::inv(s);
        std::cerr << result << std::endl;
        assertSerieEqual(result, {
            -0.121212, 0.454545, -0.212121,
            0.606061, -0.272727, -0.0909091,
            -0.575758, -0.0909091, 0.242424},
        1e5);
    }
}
