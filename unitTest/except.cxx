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

#include <iostream>
#include <dataframe/Serie.h>
#include <dataframe/Dataframe.h>
#include <dataframe/functional/math/add.h>
#include "assertions.h"


int main()
{
    df::Serie a(2, {1, 2, 3, 4});
    df::Serie b(2, {4, 3, 2, 1, 3, 3});
    df::Serie c(3, {4, 3, 2, 1, 3, 3}); // different itemSize from previous

    /**
     * @brief Different count
     */
    shouldThrowError([a, b]() {
        df::math::add({a, b});
    });

    /**
     * @brief Different itemSize
     */
    shouldThrowError([a, c]() {
        df::math::add({a, c});
    });

    /**
     * @brief Add in a dataframe with different count
     */
    shouldNotThrowError([a, b]() {
        // Not same count
        df::Dataframe dataframe ;
        dataframe.add("pos", a);
        dataframe.add("idx", b);
    });

    return 0;
}
