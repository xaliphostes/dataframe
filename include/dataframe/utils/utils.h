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

#pragma once

#include <string>
#include <iostream>

namespace df
{

    inline void info(const std::string &msg)
    {
        std::cout << "Info: " << msg << std::endl;
    }

    inline void warning(const std::string &msg)
    {
        std::cerr << "Warn: " << msg << std::endl;
    }

    inline void error(const std::string &msg)
    {
        std::cerr << "Error: " << msg << std::endl;
    }

    template <typename Arg, typename... Args>
    inline void log(Arg &&arg, Args &&...args)
    {
        std::cout << std::forward<Arg>(arg);
        using expander = int[];
        (void)expander{0, (void(std::cout << ' ' << std::forward<Args>(args)), 0)...};
    }

    template<typename T, typename Func>
    std::vector<T> map(const std::vector<T>& input, Func func) {
        std::vector<T> result;
        result.reserve(input.size());
        for (const auto& element : input) {
            result.push_back(func(element));
        }
        return result;
    }

    template <typename C>
    bool contains(const std::vector<C>& vec, const C& item) {
        return std::find(vec.begin(), vec.end(), item) != vec.end() ;
    }

}