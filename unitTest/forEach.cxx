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
#include <dataframe/functional/forEach.h>
#include <dataframe/functional/math/div.h>
#include "assertions.h"

int main()
{
    {
        // Pour Serie scalaire
        df::Serie s1(1, {1, 2, 3});
        df::forEach(s1, [](double v, uint32_t i)
                { std::cout << "Value " << i << ": " << v << "\n"; });

        // Pour Serie non-scalaire
        df::Serie s2(3, {1, 2, 3, 4, 5, 6});
        df::forEach(s2, [](const Array &v, uint32_t i)
                { std::cout << "Vector " << i << ": " << v << "\n"; });

        // Avec déduction automatique du type
        df::forEach(s2, [](const auto &v, uint32_t i)
                { std::cout << "Item " << i << ": " << v << "\n"; });
    }

    // makeForEach pour créer des fonctions réutilisables
    {
        auto printer = df::makeForEach([](const auto &v, uint32_t i)
                                   { std::cout << "Item " << i << ": " << v << "\n"; });

        df::Serie s1(1, {1, 2, 3});
        df::Serie s2(3, {1, 2, 3, 4, 5, 6});

        printer(s1); // Fonctionne avec Serie scalaire
        printer(s2); // Fonctionne avec Serie non-scalaire
    }

    // forEachIndexed avec plus d'informations sur l'index
    {
        df::Serie s(1, {1, 2, 3});
        df::forEachIndexed(s, [](const auto &v, uint32_t i, bool isFirst, bool isLast)
                       { std::cout << "Item " << i
                                   << (isFirst ? " (first)" : "")
                                   << (isLast ? " (last)" : "")
                                   << ": " << v << "\n";
        });
    }

#ifndef __APPLE__
    // forEachParallel pour le traitement parallèle
    {
        df::Serie s(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});
        df::forEachParallel(s, [](const auto &v, uint32_t i) {
            // Sera exécuté en parallèle
            heavyComputation(v); 
        );
    }
#endif

    return 0;
}
