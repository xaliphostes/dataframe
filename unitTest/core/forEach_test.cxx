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

#include "../TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/forEach.h>
#include <dataframe/map.h>
#include <dataframe/reduce.h>

// ------------------------------------------------

df::Serie<int> scalars{1, 2, 3, 4, 5};

df::Serie<Stress3D> stress{
    {1, 2, 3, 4, 5, 6},       {7, 8, 9, 10, 11, 12},
    {13, 14, 15, 16, 17, 18}, {19, 20, 21, 22, 23, 24},
    {25, 26, 27, 28, 29, 30}, {31, 32, 33, 34, 35, 36},
};

// ------------------------------------------------

TEST(forEach, forEach) {
    df::forEach(
        [](const Stress3D &s, size_t index) {
            std::cerr << "stress " << index << ": " << s << std::endl;
        },
        stress);

    stress.forEach([](const Stress3D &s, size_t index) {
        std::cerr << "stress " << index << ": " << s << std::endl;
    });
}

TEST(forEach, one_arg) {
    df::forEach(
        [](const Stress3D &s) {
            std::cerr << "stress: " << s << std::endl;
        },
        stress);

    stress.forEach([](const Stress3D &s, size_t index) {
        std::cerr << "stress " << index << ": " << s << std::endl;
    });
}

TEST(forEach, make_forEach) {
    auto display = df::bind_forEach([](const Stress3D &s, size_t index) {
        std::cerr << "stress " << index << ": " << s << std::endl;
    });

    display(stress);
}

// TEST(forEach, forEach_multiple_series) {
//     df::forEach(
//         [](const Stress3D &matrix, const int& scalar, size_t) {
//             std::cerr << scalar << ": " << matrix << std::endl;
//         },
//         stress, scalars);
// }

RUN_TESTS();