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
#include <dataframe/functional/filter.h>
#include <iostream>

int main() {
    {
        // Filtrage scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});
        auto evens =
            df::filter([](double v, uint32_t) { return (int)v % 2 == 0; }, s1);

        // Filtrage vectoriel
        df::Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});
        auto filtered =
            df::filter([](const Array &v, uint32_t) { return v[0] > 3; }, s2);
    }

    // ----------------------------------------------

    {
        // Pour une Serie scalaire
        df::Serie s1(1, {1, 2, 3, 4, 5});

        // Création d'un filtre réutilisable pour les nombres pairs
        auto evenFilter = df::make_filter(
            [](double v, uint32_t) { return static_cast<int>(v) % 2 == 0; });

        // Création d'un filtre pour les valeurs supérieures à une limite
        auto greaterThan3 =
            df::make_filter([](double v, uint32_t) { return v > 3; });

        // Application des filtres
        auto evens = evenFilter(s1); // Sera {2, 4}
        assertSerieEqual(evens, Array{2, 4});

        auto above3 = greaterThan3(s1); // Sera {4, 5}
        assertSerieEqual(above3, Array{4, 5});
    }

    // ----------------------------------------------

    {
        // Pour une Serie vectorielle
        df::Serie s2(3,
                     {1, 2, 3, 4, 5, 6, 7, 8, 9}); // 3 vecteurs de dimension 3

        // Filtre pour les vecteurs dont la norme est supérieure à une valeur
        auto normFilter = df::make_filter([](const Array &v, uint32_t) {
            double normSquared = 0;
            for (double x : v)
                normSquared += x * x;
            return std::sqrt(normSquared) > 10.0;
        });

        // Filtre pour les vecteurs dont la première composante est positive
        auto xAbove =
            df::make_filter([](const Array &v, uint32_t) { return v[0] >= 4; });

        // Application des filtres
        auto highNorm = normFilter(s2); // [7, 8, 9]
        assertSerieEqual(highNorm, Array{7, 8, 9});

        auto posFirst = xAbove(s2); // [4, 5, 6,   7, 8, 9]
        assertSerieEqual(posFirst, {4, 5, 6, 7, 8, 9});

        // Les filtres peuvent être utilisés sur différentes Series
        df::Serie s3(3, {10, 0, 0, 10, 11, 12, 13, 14, 15});
        auto moreHighNorm = normFilter(s3); // [10, 11, 12, 13, 14, 15]
        assertSerieEqual(moreHighNorm, {10, 11, 12, 13, 14, 15});
    }

    {
        df::Serie stress(6, {1, 2, 3, 4, 5, 6, 6, 5, 4, 3, 2, 1});
        df::Serie positions(3, {1, 2, 3, 3, 2, 1});
        //auto [filtered_stress, filtered_positions] = df::filter(
        auto filtered = df::filterAll(
            [](const Array &s, const Array &p) {
                return s[0] < 0 &&
                       p[2] > 0; // Remove where both conditions are true
            },
            stress, positions);
    }

    // Filtering multiple series at the same time
    {
        double cohesion = 0.1;
        double friction_angle = 30 * M_PI / 180;

        // Compute the critical stress state
        auto computeCriticalityIndex = [=](const df::Serie &stress,
                                           const df::Serie &positions) {
            df::Serie result(1, stress.count());

            for (uint32_t i = 0; i < stress.count(); ++i) {
                const Array &values = stress.get<Array>(i);
                const Array &pos = positions.get<Array>(i);

                double sigma1 = values[0];
                double sigma3 = values[2];
                double deviatoric = sigma1 - sigma3;

                double critical_stress = 2 * cohesion *
                                         std::cos(friction_angle) /
                                         (1 - std::sin(friction_angle));

                double depth_factor = std::exp(-(-pos[2]) / 1000.0);
                result.set(i, (deviatoric / critical_stress) * depth_factor);
            }
            return result;
        };

        // Input data
        df::Serie stress(
            6, {-2, 4, 6, -3, 6, -9, 1, 2, 3, 4, 5, 6, 9, 8, 7, 6, 5, 4});
        df::Serie positions(3, {10, 20, -30, 1, 0, 0, 2, 0, 0});
        df::Serie markers(1, {1, 2, 2});

        // Filter based on multiple conditions
        auto filtered = df::filterAll(
            [](const Array &s, const Array &p, const Array &m) {
                return s[0] < 0 && // compressive stress
                       p[2] < 0 && // depth condition
                       m[0] == 1;  // specific rock type
            },
            stress, positions, markers);

        // Access filtered Series
        auto filtered_stress = filtered[0];
        auto filtered_positions = filtered[1];
        auto filtered_markers = filtered[2];

        assertSerieEqual(filtered_stress, df::Serie(6, {-2, 4, 6, -3, 6, -9}));
        assertSerieEqual(filtered_positions, df::Serie(3, {10, 20, -30}));
        assertSerieEqual(filtered_markers, df::Serie(1, {1}));

        // Use in pipeline
        auto result1 = df::pipe(
            df::filterAll([](const Array &s,
                             const Array &p) { return s[0] < 0 && p[2] < 0; },
                          stress, positions),
            [=](const df::Series &fs) {
                return computeCriticalityIndex(fs[0], fs[1]);
            });

        assertSerieEqual(result1, df::Serie(1, {-22.4115}), 1e-4);
    }

    return 0;
}
