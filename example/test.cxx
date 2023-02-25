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
#include "../src/Dataframe.h"

int main()
{
    Dataframe dataframe;

    Array array = {0, 1, 3, 2, 7, 8, 7, 6, 9, 7, 4, 3, 2, 8, 5};
    Serie positions = Serie(3, array);
    dataframe.add("positions", positions);

    Serie indices = positions.map([](const Array &t, uint32_t i)
                                  { return Array({t[0] * t[0], t[1] * t[1] + t[2] * t[2]}); });
    dataframe.add("indices", indices);

    dataframe["positions"].dump();
    dataframe["indices"].dump();

    std::cerr << std::endl;

    positions
        .map([](const Array &t, uint32_t i)
             {
            double norm = std::sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]) ;
            return Array({norm}) ; })
        .forEach([](Array v, uint32_t i)
                 { std::cerr << "[" << i << "]: " << v << std::endl; });

    // positions
    //     .map([](const Array &t, uint32_t i) {
    //         double norm = std::sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]) ;
    //         return Array({norm}) ; })
    //     .reduce([](Array acc, const Array& v) {
    //         return v[0] + acc[0];
    //     }, Array({0})) ;
}
