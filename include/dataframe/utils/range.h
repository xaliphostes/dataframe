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
 * @brief Creates a Serie with consecutive numbers
 * @param start The starting value
 * @param end The ending value (exclusive)
 * @param step The increment between values (default = 1)
 * @return Serie<T> containing the range of values
 *
 * @code
 * // Examples of usage:
 * auto s1 = df::range(5);             // [0,1,2,3,4]
 * auto s2 = df::range(2, 6);          // [2,3,4,5]
 * auto s3 = df::range(0, 10, 2);      // [0,2,4,6,8]
 * auto s4 = df::range(5.0, 7.0, 0.5); // [5.0,5.5,6.0,6.5]
 * @endcode
 */
template <typename T = int> Serie<T> range(T start, T end, T step = 1);

/**
 * @brief Creates a Serie with consecutive numbers from 0 to end-1
 * @param end The ending value (exclusive)
 * @return Serie<T> containing the range of values
 */
template <typename T = int> Serie<T> range(T end);

/**
 * Helper function for pipe operations.
 *
 * @code
 * // Using with pipe
 * auto result = pipe(
 *     df::range(0, 10),
 *     bind_map([](int x) { return x * 2; })
 * );
 * @endcode
 */
template <typename T> auto bind_range(T start, T end, T step = 1);

} // namespace df

#include "inline/range.hxx"