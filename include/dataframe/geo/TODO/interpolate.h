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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/common.h>

namespace df {
namespace geo {

/**
 * @brief Interpolates field values at arbitrary points using linear
 * interpolation
 * @param field The field values serie (can be scalar or vector)
 * @param positions The positions serie where field values are known
 * @param query_points The points where we want to interpolate values
 * @return Serie<T> Interpolated values at query points
 */
template <typename T>
Serie<T> interpolate(const Serie<T> &field, const Serie<T> &positions,
                        const Serie<T> &query_points);

// Helper function to create an interpolation operation
template <typename T> auto make_interpolate(const Serie<T> &positions);

} // namespace geo
} // namespace df

#include "inline/interpolate.hxx"