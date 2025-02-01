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
 * // For arithmetic types
 * Serie<double> s1{1.5, -2.5, 3.5, -4.5, 5.5};
 * auto [min_val, max_val] = bounds(s1);  // Gets both bounds
 * auto min_only = min(s1);               // Gets just minimum
 * auto max_only = max(s1);               // Gets just maximum
 *
 * // For Vector3D
 * Serie<Vector3D> s2{{1.0, -2.0, 3.0}, {-4.0, 5.0, -6.0}};
 * auto [vec_min, vec_max] = bounds(s2);  // Gets min/max across all components
 *
 * // Using pipeline syntax
 * auto [pipe_min, pipe_max] = s1 | bind_bounds();
 * auto min_pipe = s1 | bind_min();
 * auto max_pipe = s1 | bind_max();
 * @endcode
 */

namespace df {

template <typename T> auto bounds(const Serie<T> &serie);
template <typename T> auto min(const Serie<T> &serie);
template <typename T> auto max(const Serie<T> &serie);

auto bind_bounds();
auto bind_min();
auto bind_max();

} // namespace df

#include "inline/bounds.hxx"