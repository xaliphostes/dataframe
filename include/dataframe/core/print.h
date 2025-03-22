/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>

/**
 * @code
 * // Basic usage with zip
 * Serie<double> s1{1.23456, 2.34567, 3.45678};
 * Serie<int> s2{10, 20, 30};
 * Serie<float> s3{100.123f, 200.234f, 300.345f};
 * 
 * print(zip(s1, s2, s3));  // default precision
 * print(zip(s1, s2, s3), 2);  // custom precision
 * 
 * // Pipeline operation
 * zip(s1, s2, s3) | bind_print<double, int, float>(3);
 * 
 * // Zip with expressions
 * auto scaled = s1.map([](double x) { return x * 2; });
 * print(zip(s1, scaled));
 * @endcode
 *
 * @code
 * // Single series
 * Serie<double> s1{1.23456, 2.34567, 3.45678};
 * print(s1);  // default precision (4)
 * print(s1, 2);  // custom precision
 * 
 * // Multiple series
 * std::vector<Serie<double>> series{s1, s2, s3};
 * print(series);  // default precision
 * print(series, 2);  // custom precision
 * 
 * // Pipeline operation
 * s1 | bind_print<double>(3);
 * @endcode
 */

namespace df {

template <typename... Types> using ZippedSeries = Serie<std::tuple<Types...>>;

template <typename T> void print(const std::vector<Serie<T>> &, size_t = 4);
template <typename T> void print(const Serie<T> &, size_t = 4);

// Print function for zipped series
template <typename... Types>
void print(const ZippedSeries<Types...> &, size_t = 4);

template <typename T> auto bind_print(size_t precision = 4);
template <typename... Types> auto bind_print_zipped(size_t = 4);

} // namespace df

#include "inline/print.hxx"
