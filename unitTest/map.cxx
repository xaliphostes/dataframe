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

#include <dataframe/Serie.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/zip.h>
#include <dataframe/functional/map.h>
#include "assertions.h"

// These functions can be combined with the pipe and zip functions:
void doPipe()
{
    df::Serie s1(1, {1, 2, 3});
    df::Serie s2(2, {4, 5, 6, 7, 8, 9});

    // Combine multiple operations
    auto result = df::pipe(
        df::zip(s1, s2),
        df::make_map([](const Array &v, uint32_t)
                    {
            Array out(v.size());
            for(size_t j = 0; j < v.size(); ++j) {
                out[j] = v[j] * 2;
            }
            return out; }));

    // Create reusable transformations
    auto pipeline = df::make_pipe(
        df::make_map([](double v, uint32_t)
                    { return v * 2; }),
        df::make_map([](double v, uint32_t)
                    { return v + 1; }));

    auto transformed = pipeline(s1);
}

int main()
{

    {
        auto doubler = df::make_map([](double v, uint32_t) { return v * 2; });
        df::Serie s(1, {1, 2, 3, 4, 5});
        auto result = doubler(s);
        assertSerieEqual(result, {2, 4, 6, 8, 10});
    }

    {
        auto norm = df::make_map([](const Array &v, uint32_t)
                                { return std::sqrt(v[0] * v[0] + v[1] * v[1]); });
        df::Serie s(2, {1, 2, 3, 4, 5, 6});
        auto result = norm(s); // [2.24, 5, 7.81]
        assertSerieEqual(result, Array{2.236, 5, 7.81025}, 1e-4);
    }

    {
        df::Serie s1(1, {1, 2, 3, 4});

        // Scalar → Scalar
        auto doubled = map(s1, [](double v, uint32_t) {
            return v * 2; // Retourne un double
        }); // [2, 4, 6, 8]
        assertSerieEqual(doubled, {2, 4, 6, 8});

        // Scalar → Vector
        auto expanded = map(s1, [](double v, uint32_t) {
            return Array{v, v * v, v * v * v}; // Retourne un Array
        }); // [1, 1, 1, 2, 4, 8, 3, 9, 27, 4, 16, 64]
        assertSerieEqual(expanded, {1, 1, 1, 2, 4, 8, 3, 9, 27, 4, 16, 64});

        df::Serie s2(3, {1, 2, 3, 4, 5, 6});

        // Vector → Scalar
        auto norms = map(s2, [](const Array &v, uint32_t) {
            double sum = 0;
            for (double x : v)
                sum += x * x;
            return std::sqrt(sum); // Retourne un double
        }); // [3.74166, 8.77496]
        assertSerieEqual(norms, Array{3.74166, 8.77496}, 1e-4);

        // Vector → Vector
        auto scaled = map(s2, [](const Array &v, uint32_t) {
            Array result(v.size() * 2);
            for (size_t i = 0; i < v.size(); ++i)
            {
                result[i * 2] = v[i];
                result[i * 2 + 1] = v[i] * 2;
            }
            return result; // Retourne un Array
        }); // [1, 2, 2, 4, 3, 6, 4, 8, 5, 10, 6, 12]
        assertSerieEqual(scaled, Array{1, 2, 2, 4, 3, 6, 4, 8, 5, 10, 6, 12});
    }

    doPipe();
}
