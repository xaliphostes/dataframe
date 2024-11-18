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

    const df::Serie &toto = dataframe["toto"];

    String name = df::nameOfSerie(dataframe, toto);
    assertEqual(name, String("toto"));

    df::Serie serie(1, {1, 2, 3, 4});
    name = df::nameOfSerie(dataframe, serie);
    assertEqual(name, String(""));

    serie.forEach([](const Array &a, uint32_t index) {
        std::cerr << index << "  " << a << std::endl ;
        assertCondition(a.size()==1, "size should be 1");
        assertCondition(a[0]==index+1, "value should be " + std::to_string(index+1) + ". Got " + std::to_string(a[0]) + "!");

        
    });

    // We know the serie is made of scalar values, so use it for performance reasons...
    //
    serie.forEachScalar([](double a, uint32_t index) {
        std::cerr << index << "  " << a << std::endl;
        assertCondition(a==index+1, "value should be " + std::to_string(index+1) + ". Got " + std::to_string(a) + "!");
    });

    return 0;
}
