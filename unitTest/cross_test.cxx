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
#include <dataframe/functional/algebra/cross.h>

template <typename T>
void crossTest(const Array<T> &A, const Array<T> &B, const Array<T> &sol) {
    auto r = df::algebra::cross(df::GenSerie<T>(3, A), df::GenSerie<T>(3, B));
    EXPECT_ARRAY_NEAR(r.asArray(), sol, 1e-10);
}

TEST(cross, _1) {
    crossTest<float>({2, 3, 4, 5, 6, 7}, {5, 6, 7, -1, 4, 2},
                     {-3, 6, -3, -16, -17, 26});

    crossTest<double>({2, 3, 4, 5, 6, 7}, {5, 6, 7, -1, 4, 2},
                      {-3, 6, -3, -16, -17, 26});

    crossTest<long double>({2, 3, 4, 5, 6, 7}, {5, 6, 7, -1, 4, 2},
                           {-3, 6, -3, -16, -17, 26});
}

TEST(cross, _2) {
    {
        // 2D cross product (retourne un scalaire)
        df::GenSerie<double> s1(2, {1, 0, 0, 1, -1, 0}); // Vecteurs 2D
        df::GenSerie<double> s2(2, {0, 1, -1, 0, 0, -1});
        auto result = df::algebra::cross(s1, s2); // itemSize == 1
        auto sol = Array<double>{1.0000, 1.0000, 1.0000};
        EXPECT_ARRAY_NEAR(result.asArray(), sol, 1e-10);
    }

    {
        // 3D cross product (retourne des vecteurs)
        df::GenSerie<double> v1(3, {1, 0, 0, 0, 1, 0}); // Vecteurs 3D
        df::GenSerie<double> v2(3, {0, 1, 0, -1, 0, 0});
        auto result = df::algebra::cross(v1, v2); // itemSize == 3
        auto sol =
            Array<double>{0.0000, 0.0000, 1.0000, 0.0000, -0.0000, 1.0000};
        EXPECT_ARRAY_NEAR(result.asArray(), sol, 1e-10);
    }

    {
        // Cross product avec un vecteur constant
        df::GenSerie<double> v1(3, {1, 0, 0, 0, 1, 0}); // Vecteurs 3D
        std::vector<double> constant = {0, 0, 1};
        auto result = df::algebra::cross(v1, constant);
        auto sol =
            Array<double>{0.0000, -1.0000, 0.0000, 1.0000, 0.0000, 0.0000};
        EXPECT_ARRAY_NEAR(result.asArray(), sol, 1e-10);
    }
}

RUN_TESTS()