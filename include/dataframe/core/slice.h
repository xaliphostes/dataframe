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
#include <stdexcept>

/**
 * @brief Extract a subset of elements from a Serie
 *
 * @code
 * // Example usage:
 * df::Serie<int> serie{0, 1, 2, 3, 4, 5};
 *
 * // Basic slicing
 * auto result1 = df::slice(serie, 2, 4);  // [2, 3]
 *
 * // Slicing from start
 * auto result2 = df::slice(serie, 3);     // [0, 1, 2]
 *
 * // Slicing with step
 * auto result3 = df::slice(serie, 0, 6, 2);  // [0, 2, 4]
 *
 * // Using bind_slice in a pipeline
 * auto result4 = serie | df::bind_slice(2, 4);  // [2, 3]
 * @endcode
 */

namespace df {

/**
 * @brief Slice a Serie from start index to end index
 *
 * @tparam T The type of elements in the Serie
 * @param serie The input Serie to slice
 * @param start The starting index (inclusive)
 * @param end The ending index (exclusive)
 * @return Serie<T> A new Serie containing the sliced elements
 * @throws std::invalid_argument if start > end
 * @throws std::out_of_range if end > serie.size()
 */
template <typename T>
Serie<T> slice(const Serie<T> &serie, size_t start, size_t end);

/**
 * @brief Slice a Serie from index 0 to end index
 *
 * @tparam T The type of elements in the Serie
 * @param serie The input Serie to slice
 * @param end The ending index (exclusive)
 * @return Serie<T> A new Serie containing the sliced elements
 * @throws std::out_of_range if end > serie.size()
 */
template <typename T> Serie<T> slice(const Serie<T> &serie, size_t end);

/**
 * @brief Slice a Serie from start to end with a step size
 *
 * @tparam T The type of elements in the Serie
 * @param serie The input Serie to slice
 * @param start The starting index (inclusive)
 * @param end The ending index (exclusive)
 * @param step The step size between elements
 * @return Serie<T> A new Serie containing the sliced elements
 * @throws std::invalid_argument if start > end or step == 0
 * @throws std::out_of_range if end > serie.size()
 */
template <typename T>
Serie<T> slice(const Serie<T> &serie, size_t start, size_t end, size_t step);

/**
 * @brief Create a bound slice function for use in pipelines
 *
 * @tparam Args Types of the slice arguments
 * @param args The slice arguments (start, end, and optionally step)
 * @return A function that takes a Serie and returns a sliced Serie
 */
template <typename... Args> auto bind_slice(Args... args);

} // namespace df

#include "inline/slice.hxx"