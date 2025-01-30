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
#include <dataframe/utils.h>

namespace df {

/**
 * Create a Serie with n random samples between min and max (inclusive)
 * @param n Number of samples
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive)
 * @return Serie<T> containing n random samples
 *
 * Usage:
 * @code
 * // Generate 100 random integers between -10 and 10
 * auto ints = df::random_int(100, -10, 10);
 *
 * // Generate 1000 random doubles between 0 and 1
 * auto doubles = df::random_double(1000, 0.0, 1.0);
 *
 * // Generic usage with any numeric type
 * auto floats = df::random<float>(50, -1.0f, 1.0f);
 *
 * // Use with pipe operations
 * auto processed = df::random_double(100, 0.0, 1.0)
 *     | df::bind_map([](double v, size_t) { return v * 2.0; })
 *     | df::bind_filter([](double v, size_t) { return v > 1.0; });
 * @endcode
 */
template <typename T> Serie<T> random(size_t n, T min, T max);

} // namespace df

#include "inline/random.hxx"