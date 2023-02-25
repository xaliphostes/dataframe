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

int main()
{
    Dataframe dataframe ;

    Array array = {0,1,3,2,7,8,7,6, 9,7,4,3,2,8,5,5} ;
    dataframe.add("positions", Serie(8, array)) ;

    dataframe.add("indices", dataframe["positions"].map([](const Array &t, uint32_t i) {
        return Array({t[0] * t[0] , t[1] * t[1] + t[2] * t[2]}) ;
    }) ) ;

    dataframe["positions"].dump() ;
    dataframe["indices"].dump() ;

    std::cerr << std::endl ;

    dataframe["positions"].forEach( [](Array v, uint32_t i) {
        std::cerr << "[" << i << "]: " << v << std::endl ;
    }) ;

    std::cerr << std::endl ;
    std::cerr << dataframe["indices"].array() << std::endl ;

    dataframe.dump() ;
}
