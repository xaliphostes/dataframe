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
#include <dataframe/math/eigen.h>
#include "assertions.h"

int main()
{
    // sym matrix 3x3 => itemSize=6
    // 3 items
    df::Serie serie(6, {2, 4, 6, 3, 6, 9, 1, 2, 3, 4, 5, 6, 9, 8, 7, 6, 5, 4});
    auto values = df::eigenValues(serie);
    auto vectors = df::eigenVectors(serie);

    std::vector<Array> vals{
        {16.3328, -0.658031, -1.67482},
        {11.3448, 0.170914, -0.515728},
        {20.1911, -0.043142, -1.14795}
    };

    values.forEach([vals](const Array &v, uint32_t i) {
        assertArrayEqual(vals[i], v, 1e-4);
    });

    std::vector<Array> vecs{
        {0.449309, 0.47523, 0.75649, 0.194453, 0.774452, -0.602007, 0.871957, -0.417589, -0.255559},
        {0.327985, 0.591009, 0.736977, -0.592113, 0.736484, -0.327099, 0.73609, 0.32909, -0.5915},
        {0.688783, 0.553441, 0.468275, 0.15941, -0.745736, 0.64689, -0.707225, 0.370919, 0.601874}
    };

    vectors.forEach([vecs](const Array &v, uint32_t i) {
        assertArrayEqual(vecs[i], v, 1e-4);
    });

    return 0;
}
