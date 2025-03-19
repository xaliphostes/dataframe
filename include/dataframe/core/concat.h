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

/**
 * @code
 * // Example usage:
 * df::Serie<double> s1({1.0, 2.0, 3.0});
 * df::Serie<double> s2({4.0, 5.0, 6.0});
 * df::Serie<double> s3({7.0, 8.0, 9.0});
 *
 * // Using variadic version
 * auto result1 = df::concat(s1, s2, s3);
 *
 * // Using vector version
 * std::vector<df::Serie<double>> series_vec{s1, s2, s3};
 * auto result2 = df::concat(series_vec);
 * @endcode
 */

namespace df {

template <typename T> Serie<T> concat(const std::vector<Serie<T>> &series);

/**
 * Variadic template version for ease of use
 */
template <typename T, typename... Args>
Serie<T> concat(const Serie<T> &first, const Args &...args);

/**
 * @code
 * // Example usage:
 * df::Serie<double> s1({1.0, 2.0, 3.0});
 * df::Serie<double> s2({4.0, 5.0, 6.0});
 * df::Serie<double> s3({7.0, 8.0, 9.0});
 *
 * // This will concatenate s1, s2, and s3
 * //
 * auto result1 = bind_concat(s1, s2, s3);
 * auto result2 = bind_concat({s1, s2, s3});
 * auto result3 = s1 | bind_concat(s2, s3);
 * @endcode
 */
template <typename... Args> auto bind_concat(const Args &...series);

} // namespace df

#include <dataframe/core/inline/concat.hxx>