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
 * @brief Concatenate multiple Serie collections into a single Serie
 *
 * Creates a new Serie by appending all elements from each input Serie in order.
 * All Serie collections must contain elements of the same type.
 *
 * @tparam T The type of elements in the Series
 * @param first The first Serie to include in the chain
 * @param rest Additional Series to append to the chain
 * @return Serie<T> A new Serie containing all elements from the input Series
 *
 * Example usage:
 * @code
 * // Basic chaining
 * Serie<int> s1{1, 2, 3};
 * Serie<int> s2{4, 5, 6};
 * Serie<int> s3{7, 8, 9};
 * auto result = chain(s1, s2, s3); // Result: {1, 2, 3, 4, 5, 6, 7, 8, 9}
 *
 * // Chain with empty Series
 * Serie<int> empty;
 * auto result2 = chain(empty, s1, empty, s2); // Result: {1, 2, 3, 4, 5, 6}
 *
 * // With pipe syntax
 * auto result3 = s1 | bind_chain(s2, s3);
 * @endcode
 */
template <typename T, typename... Args>
Serie<T> chain(const Serie<T> &first, const Args &...rest);

/**
 * @brief Chain two Series together (binary version)
 *
 * Creates a new Serie by appending the second Serie to the first.
 *
 * @tparam T The type of elements in the Series
 * @param first The first Serie
 * @param second The second Serie to append
 * @return Serie<T> A new Serie containing elements from both input Series
 */
template <typename T>
Serie<T> chain(const Serie<T> &first, const Serie<T> &second);

/**
 * @brief Creates a function object that can be used with pipe operations to
 * chain Series
 *
 * @tparam Args Types of additional Serie arguments
 * @param rest Additional Series to append to the chain
 * @return A function object compatible with df::pipe
 */
template <typename... Args> auto bind_chain(const Args &...rest);

} // namespace df

#include "inline/chain.hxx"
