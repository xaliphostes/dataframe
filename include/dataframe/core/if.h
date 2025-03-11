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
#include <dataframe/utils/utils.h>
#include <functional>

namespace df {

/**
 * @brief Apply conditional transformation to each element of a Serie
 *
 * For each element in the input Serie, applies either the 'true_transform'
 * function if the condition evaluates to true, or the 'false_transform'
 * function otherwise.
 *
 * @tparam T Type of elements in the input Serie
 * @tparam R Type of elements in the output Serie
 * @param serie Input Serie to process
 * @param condition Function that evaluates each element to determine which
 * transformation to apply
 * @param true_transform Function to apply when condition returns true
 * @param false_transform Function to apply when condition returns false
 * @return Serie<R> A new Serie with conditionally transformed elements
 *
 * Example:
 * @code
 * // Replace negative values with zero, keep positive values
 * Serie<double> values{-5.0, 3.2, -1.7, 8.1, 0.0};
 * auto result = df::if_then_else(values,
 *     [](double x) { return x < 0; },        // condition
 *     [](double) { return 0.0; },            // true transform (applied to
 * negative values)
 *     [](double x) { return x; });           // false transform (applied to
 * non-negative values)
 * // result = {0.0, 3.2, 0.0, 8.1, 0.0}
 *
 * // Using pipe syntax with index
 * auto abs_values = values | df::bind_if_then_else(
 *     [](double x, size_t) { return x < 0; },
 *     [](double x, size_t) { return -x; },
 *     [](double x, size_t) { return x; });
 * // abs_values = {5.0, 3.2, 1.7, 8.1, 0.0}
 * @endcode
 */
template <typename T, typename R>
Serie<R> if_then_else(const Serie<T> &serie,
                      std::function<bool(const T &, size_t)> condition,
                      std::function<R(const T &, size_t)> true_transform,
                      std::function<R(const T &, size_t)> false_transform);

/**
 * @brief Simplified version of if_then_else without index parameter
 */
template <typename T, typename R>
Serie<R> if_then_else(const Serie<T> &serie,
                      std::function<bool(const T &)> condition,
                      std::function<R(const T &)> true_transform,
                      std::function<R(const T &)> false_transform);

/**
 * @brief Create a reusable if_then_else pipeline operation
 */
template <typename T, typename R>
auto bind_if_then_else(std::function<bool(const T &, size_t)> condition,
                       std::function<R(const T &, size_t)> true_transform,
                       std::function<R(const T &, size_t)> false_transform);

/**
 * @brief Simplified bind_if_then_else without index parameter
 */
template <typename T, typename R>
auto bind_if_then_else(std::function<bool(const T &)> condition,
                       std::function<R(const T &)> true_transform,
                       std::function<R(const T &)> false_transform);

/**
 * @brief Helper to create a bound if_then_else operation using MAKE_OP macro
 */
MAKE_OP(if_then_else);

} // namespace df

#include "inline/if.hxx"