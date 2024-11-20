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
#include <dataframe/functional/reduce.h>
#include "assertions.h"

int main()
{
    {
        df::Serie serie(3, {1, 2, 3, 4, 5, 6}); // 2 items with itemSize = 3

        df::Serie s = df::reduce(
            serie, [](const Array &prev, const Array &cur, uint32_t)
            { return Array{prev[0] + cur[0], prev[1] + cur[1]}; }, Array{10, 20});

        Array sol{10 + 1 + 4, 20 + 2 + 5}; // 2 items with itemSize = 2
        assertArrayEqual(s.asArray(), sol);
        std::cerr << s.asArray() << std::endl;

        s = df::reduce(
            serie, [](const Array &prev, const Array &cur, uint32_t)
            { return Array{prev[0] + cur[0]}; }, Array{0});
        std::cerr << s.asArray() << std::endl;
    }

    {
        // Réduction scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});
        auto sum = df::reduce(s1, [](double acc, double v, uint32_t) {
            return acc + v; },
        0.0);

        // Réduction vectorielle
        df::Serie s2(3, {1, 2, 3, 4, 5, 6});
        df::Serie vectorSum = df::reduce(s2, [](const Array &acc, const Array &v, uint32_t)
                                         {
        Array result(acc.size());
        for(size_t i = 0; i < acc.size(); ++i) {
            result[i] = acc[i] + v[i];
        }
        return result; }, Array(3, 0.0)); // Retourne une Serie
    }

    return 0;
}
