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

#include <iostream>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/Dataframe.h>
#include <dataframe/utils/nameOfSerie.h>
#include "assertions.h"

int main()
{
    df::Dataframe dataframe;
    dataframe.add("toto", df::Serie(1, {1, 2, 3, 4}));

    const df::Serie &toto = dataframe["toto"];

    String name = df::nameOfSerie(dataframe, toto);
    assertEqual(name, String("toto"));

    df::Serie serie(1, {1, 2, 3, 4});
    name = df::nameOfSerie(dataframe, serie);
    assertEqual(name, String(""));

    serie.forEach([](double a, uint32_t index)
                  {
                      std::cerr << index << "  " << a << std::endl;
                  });

    // We know the serie is made of scalar values, so use it for performance reasons...
    //
    serie.forEach([](double a, uint32_t index)
                        {
        std::cerr << index << "  " << a << std::endl;
        assertCondition(a==index+1, "value should be " + std::to_string(index+1) + ". Got " + std::to_string(a) + "!"); });

    {
        // Pour une Serie scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});

        // Plusieurs façons d'accéder aux valeurs
        double v1 = s1.get<double>(0); // Explicite
        auto v2 = s1.get(0);           // Implicite
        double v3 = s1.scalar(0);      // Méthode legacy

        // Plusieurs façons de modifier les valeurs
        s1.set(0, 42.0);       // Nouvelle méthode
        s1.setScalar(0, 42.0); // Méthode legacy

        // Pour une Serie non-scalaire
        df::Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});

        // Plusieurs façons d'accéder aux valeurs
        Array v4 = s2.get<Array>(0); // Explicite
        auto v5 = s2.get(0);         // Implicite
        Array v6 = s2.value(0);      // Méthode legacy

        // Plusieurs façons de modifier les valeurs
        s2.set(0, Array{10, 11, 12});      // Nouvelle méthode
        s2.setValue(0, Array{10, 11, 12}); // Méthode legacy

        // Utilisation avec les méthodes unifiées précédentes
        s2.forEach([](const auto &v, uint32_t i) {
            // Le type de v est déduit automatiquement
            std::cout << "Item " << i << ": " << v << std::endl;
        });

        auto doubled = s2.map([](const auto &v, uint32_t)
                              {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v * 2;
        } else {
            Array result(v.size());
            for(size_t i = 0; i < v.size(); ++i) {
                result[i] = v[i] * 2;
            }
            return result;
        } });
    }

    return 0;
}
