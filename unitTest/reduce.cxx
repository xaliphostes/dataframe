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

#include "assertions.h"
#include <dataframe/Serie.h>
#include <dataframe/functional/reduce.h>
#include <iostream>

int main() {
    {
        df::Serie serie(3, {1, 2, 3, 4, 5, 6}); // 2 items with itemSize = 3

        df::Serie s = df::reduce(
            [](const Array &prev, const Array &cur, uint32_t) {
                return Array{prev[0] + cur[0], prev[1] + cur[1]};
            },
            serie, Array{10, 20});

        Array sol{10 + 1 + 4, 20 + 2 + 5}; // 2 items with itemSize = 2
        assertArrayEqual(s.asArray(), sol);
        std::cerr << s.asArray() << std::endl;

        s = df::reduce([](const Array &prev, const Array &cur,
                          uint32_t) { return Array{prev[0] + cur[0]}; },
                       serie, Array{0});
        std::cerr << s.asArray() << std::endl;
    }

    {
        // Réduction scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});
        auto sum = df::reduce(
            [](double acc, double v, uint32_t) { return acc + v; }, s1, 0.0);

        // Réduction vectorielle
        df::Serie s2(3, {1, 2, 3, 4, 5, 6});
        df::Serie vectorSum = df::reduce(
            [](const Array &acc, const Array &v, uint32_t) {
                Array result(acc.size());
                for (size_t i = 0; i < acc.size(); ++i) {
                    result[i] = acc[i] + v[i];
                }
                return result;
            },
            s2, Array(3, 0.0)); // Retourne une Serie
    }

    {
        // // Reduce multiple Series
        // df::Serie pos(3, {1, 0, 0, 0, 1, 0, 0, 0, 1});
        // df::Serie weights(1, {1, 2, 1});
        // auto center = df::_reduce(
        //     [](Array acc, const Array &p, double w, uint32_t) {
        //         for (size_t i = 0; i < acc.size(); ++i) {
        //             acc[i] += p[i] * w;
        //         }
        //         return acc;
        //     },
        //     Array(3, 0.0), // Initial value
        //     pos, weights);
    }

    return 0;
}
