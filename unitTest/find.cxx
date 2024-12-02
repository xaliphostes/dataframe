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

#include "assertions.h"
#include <dataframe/Serie.h>
#include <dataframe/functional/find.h>
#include <iostream>

using namespace df;

int main() {
    // Find first scalar matching condition
    Serie s1(1, {-1, 2, 3, -4, 5});
    auto result1 = find([](double v, uint32_t) { return v > 3; }, s1);
    if (result1) {
        std::cout << "Found " << result1.value << " at index " << result1.index
                  << "\n";
    }

    // Find first vector matching condition
    Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    auto result2 = find([](const Array &v, uint32_t) { return v[0] > 3; }, s2);
    if (result2) {
        std::cout << "Found vector at index " << result2.index << "\n";
    }

    // Find all matching elements
    auto matches1 = findAll([](double v, uint32_t) { return v > 3; }, s1);
    std::cout << matches1 << std::endl;

    auto matches2 = findAll([](const Array& v, uint32_t) { return v[0] > 3; }, s2);
    std::cout << matches2 << std::endl;

    // Using makeFind
    auto findPositiveFirst = makeFind([](const auto &v, uint32_t) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v > 0;
        } else {
            return v[0] > 0;
        }
    });

    auto result3 = findPositiveFirst(s1);
    std::cout << result3 << std::endl;

    // Using makeFind
    auto findPositive = makeFindAll([](const auto &v, uint32_t) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v > 0;
        } else {
            return v[0] > 0;
        }
    });

    auto result4 = findPositive(s1);
    std::cout << result4 << std::endl;

    return 0;
}
