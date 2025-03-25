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
 * @brief Apply a function to each element in a Serie that returns a Serie, then
 * flatten the result
 *
 * This function maps each element to a Serie using the provided mapping
 * function, then flattens all series into a single Serie. It's equivalent to
 * applying map() followed by flatten().
 *
 * @tparam T Type of elements in the input Serie
 * @tparam R Type of elements in the output Serie
 * @param serie Input Serie to process
 * @param callback Function to apply to each element, returning a Serie<R>
 * @return Serie<R> A single flattened Serie containing all elements from
 * applying the callback
 *
 * Example:
 * @code
 * // Split each string into individual characters
 * df::Serie<std::string> words{"hello", "world"};
 * auto chars = df::flatMap<std::string, char>(words, [](const std::string&
 *      word, size_t) { std::vector<char> chars(word.begin(), word.end());
 *      return df::Serie<char>(chars);
 * });
 * // Result: Serie<char>{'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd'}
 * @endcode
 */
template <typename T, typename R>
Serie<R> flatMap(const Serie<T> &serie,
                 std::function<Serie<R>(const T &, size_t)> callback);

/**
 * @brief Simplified version without index parameter
 */
template <typename T, typename R>
Serie<R> flatMap(const Serie<T> &serie,
                 std::function<Serie<R>(const T &)> callback);

/**
 * @brief Create a reusable flatMap pipeline operation with index
 */
template <typename T, typename R>
auto bind_flatMap(std::function<Serie<R>(const T &, size_t)> callback);

/**
 * @brief Create a reusable flatMap pipeline operation without index
 */
template <typename T, typename R>
auto bind_flatMap(std::function<Serie<R>(const T &)> callback);

/**
 * @brief Helper to create a bound flatMap operation using MAKE_OP macro
 */
MAKE_OP(flatMap);

} // namespace df

#include "inline/flatmap.hxx"