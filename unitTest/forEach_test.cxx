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
#include <dataframe/functional/forEach.h>

TEST(forEach, value) {

    df::GenSerie<int> s1(1, {1, 2, 3});
    df::GenSerie<int> s2(1, {10, 20, 30});

    // Callback scalar
    df::forEach(
        [](int v, uint32_t i) {
            std::cout << "Value " << i << ": " << v << "\n";
        },
        s1);

    df::forEach(
        [](int v1, int v2, uint32_t i) {
            std::cout << "Value " << i << ": " << v1 << " " << v2 << "\n";
        },
        s1, s2);

    // Lambda avec auto
    df::forEach(
        [](const auto &v, uint32_t i) {
            std::cout << "Item " << i << ": " << v << "\n";
        },
        s1);

    df::forEach(
        [](const auto &v1, const auto &v2, uint32_t i) {
            std::cout << "Item " << i << ": " << v1 << " " << v2 << "\n";
        },
        s1, s2);
}

TEST(forEach, array) {

    df::GenSerie<double> v1(3, {4, 5, 6, 7, 8, 9});
    df::GenSerie<double> v2(3, {40, 50, 60, 70, 80, 90});

    // Callback array
    df::forEach(
        [](const std::vector<double> &v, uint32_t i) {
            std::cout << "Vector " << i << ": " << v << "\n";
        },
        v1);

    df::forEach(
        [](const auto &v, uint32_t i) {
            std::cout << "Item " << i << ": " << v << "\n";
        },
        v1);

    // df::forEach(
    //     [](const std::vector<double> &v1, const std::vector<double> &v2,
    //        uint32_t i) {
    //         std::cout << "Vector " << i << ": " << v1 << " " << v2 << "\n";
    //     },
    //     v1, v2);

    // df::forEach(
    //     [](const auto &v1, const auto &v2, uint32_t i) {
    //         std::cout << "Item " << i << ": " << v1 << " " << v2 << "\n";
    //     },
    //     v1, v2);
}

RUN_TESTS()