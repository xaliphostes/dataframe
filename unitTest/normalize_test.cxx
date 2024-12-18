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

#include "TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/functional/math/normalize.h>


TEST(normalize, basic) {
    // Normalisation d'une Serie scalaire
    df::Serie s1(1, {1, 2, 3, 4, 5});
    auto normalized1 = df::math::normalize(s1); // Normalise entre 0 et 1
    assertSerieEqual(normalized1, {0, 0.25, 0.5, 0.75, 1});

    // Normalisation d'une Serie vectorielle
    df::Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    auto normalized2 = df::math::normalize(s2); // Normalise chaque vecteur à une longueur de 1
    assertSerieEqual(normalized2, {0.267261, 0.534522, 0.801784, 0.455842, 0.569803, 0.683763, 0.502571, 0.574367, 0.646162}, 1e-6);
}

RUN_TESTS()