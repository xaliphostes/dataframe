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
#include <dataframe/functional/map.h>
#include "assertions.h"

void test_scalar() {
    df::Serie serie(1, {1,2,3,4});

    serie.forEachScalar([](double v, uint32_t) {
        std::cerr << v << " " ;
    });
    std::cerr << std::endl ;

    df::Serie b = df::map(serie, [](double v, uint32_t) {
        return -1;
    });
    std::cerr << b << std::endl ;

    double c = serie.reduceScalar([](double prev, double cur, uint32_t) {
        return prev+cur;
    }, 10);
    std::cerr << c << std::endl << std::endl ;

    df::Serie d = serie.filter([](double v, uint32_t) {
        return v<=3.2;
    });
    std::cerr << d << std::endl ;
}

void other_test() {
    Array sol{1, 3, 2, 9};
    
    df::Serie a(1, {1, 3, 2, 9});

    for (uint32_t i = 0; i < a.count(); ++i)
    {
        assertEqual(a.scalar(i), sol[i]);
    }
 
    a.forEachScalar([sol](double t, uint32_t i) {
        assertEqual(t, sol[i]);
    });

    // ----------------------------------------

    df::Serie b(2, {1, 3, 2, 9});

    shouldThrowError([b](){
        b.forEachScalar([](double t, uint32_t i) {
        });
    });
}

int main()
{
    test_scalar();
    other_test();

    return 0;
}
