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
#include <dataframe/functional/find.h>

template <typename T>
void display(const df::FindResult<T> &result, const std::string &name = "") {
    std::cerr << name << std::endl;
    std::cerr << "  indices: " << result.indices << std::endl;
    std::cerr << "  values : " << result.values << std::endl << std::endl;
}

TEST(find, basic) {
    // Scalar
    df::GenSerie<double> s1(1, {1.0, 2.0, 3.0, 2.0, 4.0});

    auto result1 = df::find([](double v, uint32_t) { return v > 2.0; }, s1);
    // indices [2, 4]
    // values  [3.0, 4.0]
    display(result1, "find scalar");

    // Using helpers
    auto equal_2 = df::find_equal(s1, 2.0);
    display(equal_2, "find_equal scalar");

    auto in_range = df::find_range(s1, 2.0, 3.0);
    display(in_range, "find_range scalar");

    // Array
    df::GenSerie<double> v1(3, {1, 2, 3, 0, 2, 0, 0, 0, 3});

    auto result2 = df::find(
        [](const std::vector<double> &v, uint32_t) { return v[0] > 0; }, v1);
    display(result2, "find array");

    // Utilisation du helper pour la norme
    auto big_vectors = df::find_norm_greater(v1, 2.0);
    display(big_vectors, "find_norm_greater array");
}

TEST(find, all_scalar) {
    // Scalar
    df::GenSerie<double> s1(1, {1.0, 2.0, 3.0, 2.0, 4.0});

    // Trouver toutes les valeurs > 2
    auto result1 = df::findAll([](double v, uint32_t) { return v > 2.0; }, s1);
    // result1 contient {3.0, 4.0}
    df::print(result1);

    // Utilisation de makeFindAll
    auto finder =
        df::makeFindAll<double>([](double v, uint32_t) { return v > 2.0; });

    auto result2 = finder(s1);
    df::print(result2);
}

TEST(find, all_array) {
    // Array
    df::GenSerie<double> v1(3, {1, 0, 0, 0, 2, 0, 0, 0, 3});

    // Trouver tous les vecteurs dont la norme est > 1
    auto result1 = df::findAll(
        [](const std::vector<double> &v, uint32_t) {
            double norm = 0;
            for (auto x : v)
                norm += x * x;
            return std::sqrt(norm) > 1.0;
        },
        v1);
    df::print(result1);

    // -----------------------------------------------

    auto finderV = df::makeFindAll<double>(
        [](const std::vector<double> v, uint32_t) { return v[2] == 0.0; });

    auto result2 = finderV(v1);
    df::print(result2);
}

RUN_TESTS()
