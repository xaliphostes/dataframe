
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
#include <dataframe/functional/math/scale.h>

TEST(mult, test1) {
    df::GenSerie<double> s1(1, {1, 2, 3, 4, 5, 6});
    auto r1 = df::math::scale(s1, 2.0); // Multiplie chaque composante par 2
    df::print(r1);

    // Multiplication série scalaire par série scalaire
    df::GenSerie<double> a1(1, {1, 2, 3});
    df::GenSerie<double> a2(1, {2, 3, 4});
    auto r2 = df::math::scale(a1, a2); // Multiplication élément par élément
    df::print(r2);

    // -----------------------------------------------------------

    df::GenSerie<double> s2(3, {1, 2, 3, 4, 5, 6});
    auto r3 = df::math::scale(s2, 2.0); // Multiplie chaque composante par 2
    df::print(r3);

    // Multiplication série vectorielle par scalaire
    df::GenSerie<double> v1(3, {1, 2, 3, 4, 5, 6});
    df::GenSerie<double> v2(1, {2, 3}); // Un scalaire par vecteur
    auto r4 = df::math::scale(v1, v2);
    df::print(r4);

    // Multiplication série vectorielle par série vectorielle
    // Multiplication composante par composante
    df::GenSerie<double> v3(3, {2, 2, 2, 3, 3, 3});
    auto r5 = df::math::scale(v1, v3);
    df::print(r5);

    // Utilisation de make_mult
    auto multBy2 = df::math::make_scale(2.0);
    auto r6  = multBy2(s1);
    df::print(r6);
}

RUN_TESTS()