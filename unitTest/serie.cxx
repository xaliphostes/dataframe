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
#include <dataframe/Dataframe.h>
#include <dataframe/utils/nameOfSerie.h>
#include "assertions.h"

int main()
{
    df::Dataframe dataframe;
    dataframe.add("toto", df::Serie(1, {1, 2, 3, 4}));

    const df::Serie& toto = dataframe["toto"] ;

    String name = df::nameOfSerie(dataframe, toto) ;
    assertEqual(name, String("toto"));

    df::Serie serie(1, {1,2,3,4});
    name = df::nameOfSerie(dataframe, serie) ;
    assertEqual(name, String(""));

    serie.forEachScalar([](double v, uint32_t) {
        std::cerr << v << " " ;
    });
    std::cerr << std::endl ;

    df::Serie b = serie.mapScalar([](double v, uint32_t) {
        return -1;
    });
    std::cerr << b << std::endl ;

    double c = serie.reduceScalar([](double prev, double cur, uint32_t) {
        return prev+cur;
    }, 10);
    std::cerr << c << std::endl << std::endl ;

    df::Serie d = serie.filterScalar([](double v, uint32_t) {
        return v<=3.2;
    });
    std::cerr << d << std::endl ;

    return 0;
}
