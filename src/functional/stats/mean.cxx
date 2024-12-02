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

#include <dataframe/functional/stats/mean.h>
#include <dataframe/functional/reduce.h>
#include <dataframe/types.h>
#include <cmath>

namespace df
{

    Tuple mean(const Serie &serie)
    {
        uint32_t count = serie.count();
        Tuple m;
        if (serie.itemSize() == 1) {
            m.isNumber = true;
            m.number = reduce([](double prev, double cur, uint32_t) {
                return prev + cur;
            }, serie, 0) / count ;
            return m;
        }

        Array b = createArray(serie.itemSize(), 0);

        serie.forEach([&](const Array& a, uint32_t) {
            for (uint j = 0; j < a.size(); ++j) {
                b[j] += a[j];
            }
        });

        for (double& v: b) {
            v /= count;
        }
        
        m.array = b;
        return m;
    }

}
