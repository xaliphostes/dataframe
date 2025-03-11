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
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Generate a Serie by applying a function in a loop
 *
 * This function generates a Serie by applying a function to each index
 * in a range from start to end (exclusive) with a given step size.
 *
 * @tparam T Return type of the callback function, will be the type of the Serie
 * @param start The starting index (inclusive)
 * @param end The ending index (exclusive)
 * @param step The step size for incrementing the index
 * @param callback Function to apply at each step, receives the current index
 * @return Serie<T> A Serie containing the results of applying the callback to
 * each index
 *
 * Example:
 * @code
 * // Generate a Serie of squares from 0 to 9
 * auto squares = df::for_loop<int>(0, 10, 1, [](int i) { return i * i; });
 * // squares = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81}
 *
 * // Generate even numbers from 0 to 20
 * auto even = df::for_loop<int>(0, 21, 2, [](int i) { return i; });
 * // even = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20}
 *
 * // Create a geometric sequence
 * auto geometric = df::for_loop<double>(0, 10, 1, [](int i) { return
 * std::pow(2, i); });
 * // geometric = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512}
 * @endcode
 */
template <typename T>
Serie<T> for_loop(int start, int end, int step, std::function<T(int)>);

/**
 * @brief Creates a binder for for_loop to use in pipelines
 *
 * @tparam T Return type of the callback function
 * @param start The starting index (inclusive)
 * @param end The ending index (exclusive)
 * @param step The step size for incrementing the index
 * @return A function that takes a callback and returns a Serie<T>
 *
 * Example:
 * @code
 * // Create a pipeline that generates a range and then squares each value
 * auto squared_range = df::bind_for_loop<int>(0, 10, 1)([](int i) { return i;
 * }) | df::bind_map([](int x) { return x * x; });
 * @endcode
 */
template <typename T> auto bind_for_loop(int start, int end, int step = 1);

} // namespace df

#include "inline/for.hxx"