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

#include <dataframe/functional/stats/bins.h>
#include <dataframe/functional/math/minMax.h>
#include <dataframe/types.h>
#include <cmath>

namespace df
{

    Serie bins(const Serie &serie, uint nb)
    {
        auto [min, max] = minMax(serie);
        return bins(serie, nb, min[0], max[0]);
    }

    Serie bins(const Serie &serie, uint nb, double start, double stop)
    {
        if (serie.itemSize() != 1)
        {
            throw std::invalid_argument("bins: Serie must have itemSize=1");
            return Serie();
        }
        if (nb < 1)
        {
            return Serie();
        }

        double size = (stop - start) / double(nb);

        // binning
        Array b = createArray(nb, 0);
        serie.forEach([&](double v, uint32_t) {
            uint32_t i = std::trunc((v - start) / size);
            if (i >= nb)
            {
                i = nb - 1;
            }
            if (i < 0 || i >= nb)
            {
                throw std::invalid_argument("bins: index for bin (" + std::to_string(i) + ") out of bounds (0, "+std::to_string(nb)+")");
            }
            b[i]++;
        });

        return Serie(1, b);
    }

}
