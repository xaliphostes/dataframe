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

#include "assertions.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/functional/map.h>
#include <dataframe/functional/compose.h>
#include <dataframe/functional/algebra/eigen.h>
#include <dataframe/functional/math/normalize.h>
#include <iostream>

START_TEST(compose1) {
    auto f = [](double x) { return x * 2; };
    auto g = [](double x) { return x + 1; };
    auto h = [](double x) { return x * x; };

    auto composed = df::compose(f, g, h);
    auto result = composed(3.0); // equivalent to f(g(h(3.0)))

    assertDoubleEqual(result, 20);
}
END_TEST()

START_TEST(compose2) {
    // Example Series operations to compose
    auto computeEigenvalues = [](const df::Serie &s) {
        return df::algebra::eigenValues(s);
    };

    auto filterCompressive = [](const df::Serie &s) {
        return df::filter([](const Array &v, uint32_t) { return v[0] < 0; }, s);
    };

    auto normalizeVectors = [](const df::Serie &s) {
        return df::math::normalize(s);
    };

    // Compose them.
    // This matches the mathematical composition order (f∘g∘h)(x) = f(g(h(x))),
    // where functions are applied right to left.
    auto pipeline =
        df::compose(normalizeVectors, filterCompressive, computeEigenvalues);

    // 4 stress tensors (xx,xy,xz,yy,yz,zz)
    df::Serie stress(6, {
                            1,  0,  0, 1,  0, 1,  // Point 1
                            2,  1,  0, 2,  0, 2,  // Point 2
                            -1, 0,  0, -1, 0, -1, // Point 3
                            -2, -1, 0, -2, 0, -2  // Point 4
                        });
    auto result = pipeline(stress); // Will apply functions right to left

    std::cout << result << std::endl;
}
END_TEST()

int main() {

    compose1();
    compose2();

    return 0;
}
