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
#include <dataframe/functional/filter.h>
#include "assertions.h"

using namespace df;

int main()
{
    {
        // Filtrage scalaire
        Serie s1(1, {1, 2, 3, 4, 5});
        auto evens = filter(s1, [](double v, uint32_t)
                            { return (int)v % 2 == 0; });

        // Filtrage vectoriel
        Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});
        auto filtered = filter(s2, [](const Array &v, uint32_t)
                               { return v[0] > 3; });
    }

    // ----------------------------------------------

    {
        // Pour une Serie scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});

        // Création d'un filtre réutilisable pour les nombres pairs
        auto evenFilter = df::makeFilter([](double v, uint32_t) { return static_cast<int>(v) % 2 == 0; });

        // Création d'un filtre pour les valeurs supérieures à une limite
        auto greaterThan3 = df::makeFilter([](double v, uint32_t) { return v > 3; });

        // Application des filtres
        auto evens = evenFilter(s1);    // Sera {2, 4}
        assertSerieEqual(evens, {2,4});

        auto above3 = greaterThan3(s1); // Sera {4, 5}
        assertSerieEqual(above3, {4,5});
    }

    // ----------------------------------------------

    {
        // Pour une Serie vectorielle
        df::Serie s2(3, {1, 2, 3,   4, 5, 6,   7, 8, 9}); // 3 vecteurs de dimension 3

        // Filtre pour les vecteurs dont la norme est supérieure à une valeur
        auto normFilter = df::makeFilter([](const Array &v, uint32_t) {
            double normSquared = 0;
            for (double x : v) normSquared += x * x;
            return std::sqrt(normSquared) > 10.0;
        });

        // Filtre pour les vecteurs dont la première composante est positive
        auto xAbove = df::makeFilter([](const Array &v, uint32_t) { return v[0] >= 4; });

        // Application des filtres
        auto highNorm = normFilter(s2); // [7, 8, 9]
        assertSerieEqual(highNorm, {7,8,9});

        auto posFirst = xAbove(s2); // [4, 5, 6,   7, 8, 9]
        assertSerieEqual(posFirst, {4, 5, 6,   7, 8, 9});

        // Les filtres peuvent être utilisés sur différentes Series
        df::Serie s3(3, {10, 0, 0, 10, 11, 12, 13, 14, 15});
        auto moreHighNorm = normFilter(s3); // [10, 11, 12, 13, 14, 15]
        assertSerieEqual(moreHighNorm, {10, 11, 12, 13, 14, 15});
    }

    return 0;
}
