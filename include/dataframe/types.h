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

#pragma once
#include <vector>
#include <iostream>
#include <string>

using Array = std::vector<double>;
using String = std::string;
using Strings = std::vector<String>;

/**
 * @brief In this context of `dataframe`, a Tuple represents either
 * a number (if Serie.itemSize() == 1), or an array (if Serie.itemSize() > 1).
 */
struct Tuple {
    bool isNumber{false};
    double number;
    Array array;
};

inline Array createArray(uint32_t size, double initValue)
{
    Array a(size);
    std::fill(a.begin(), a.end(), initValue);
    return a;
}

inline std::ostream &operator<<(std::ostream &o, const Array &a)
{
    for (auto v : a)
    {
        std::cerr << v << " ";
    }
    std::cerr << std::endl;
    return o;
}

inline std::ostream &operator<<(std::ostream &o, const Strings &a)
{
    for (const String &v : a)
    {
        std::cerr << "  " << v << std::endl;
    }
    std::cerr << std::endl ;
    return o;
}
