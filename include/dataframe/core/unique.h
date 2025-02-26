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
#include <dataframe/core/ExecutionPolicy.h>

namespace df {

/**
 * @brief Remove duplicate elements from a Serie
 *
 * Creates a new Serie with duplicate elements removed, preserving the original
 * order of first occurrence for each unique element.
 *
 * @tparam T The type of elements in the Serie
 * @param serie The Serie to remove duplicates from
 * @param exec Execution policy (sequential or parallel)
 * @return Serie<T> A new Serie containing only unique elements
 *
 * Example usage:
 * @code
 * // Basic usage
 * Serie<int> values{1, 2, 3, 2, 1, 4, 5, 3};
 * auto unique_values = unique(values); // Result: {1, 2, 3, 4, 5}
 *
 * // With pipe syntax
 * auto result = values | bind_unique();
 * @endcode
 */
template <typename T>
Serie<T> unique(const Serie<T> &serie,
                ExecutionPolicy exec = ExecutionPolicy::SEQ);

/**
 * @brief Remove duplicates based on a key function
 *
 * Creates a new Serie with elements that would produce duplicate keys removed,
 * keeping only the first occurrence.
 *
 * @tparam T The type of elements in the Serie
 * @tparam KeyFunc The type of key function (must be callable with T and return
 * a comparable value)
 * @param serie The Serie to remove duplicates from
 * @param keyFn Function that extracts the key used to determine uniqueness
 * @param exec Execution policy (sequential or parallel)
 * @return Serie<T> A new Serie containing only elements with unique keys
 *
 * Example usage:
 * @code
 * // Remove duplicates based on absolute value
 * Serie<int> values{1, -1, 2, -2, 3, 4, -4};
 * auto unique_abs = unique_by(values, [](int x) { return std::abs(x); });
 * // Result: {1, 2, 3, 4} (keeps first occurrence of each absolute value)
 *
 * // Remove duplicates in objects based on a property
 * struct Person { std::string name; int age; };
 * Serie<Person> people = {...};
 * auto unique_by_age = unique_by(people, [](const Person& p) { return p.age;
 * });
 * @endcode
 */
template <typename T, typename KeyFunc>
Serie<T> unique_by(const Serie<T> &serie, KeyFunc keyFn,
                   ExecutionPolicy exec = ExecutionPolicy::SEQ);

/**
 * @brief Creates a function object that can be used with pipe operations to
 * remove duplicates
 *
 * @tparam T Type of the elements in the Serie
 * @param exec Execution policy (sequential or parallel)
 * @return A function object compatible with df::pipe
 */
template <typename T>
auto bind_unique(ExecutionPolicy exec = ExecutionPolicy::SEQ);

/**
 * @brief Creates a function object for pipe operations to remove duplicates
 * based on a key
 *
 * @tparam KeyFunc Type of the key function
 * @param keyFn Function that extracts the key used to determine uniqueness
 * @param exec Execution policy (sequential or parallel)
 * @return A function object compatible with df::pipe
 */
template <typename KeyFunc>
auto bind_unique_by(KeyFunc keyFn, ExecutionPolicy exec = ExecutionPolicy::SEQ);

} // namespace df

#include "inline/unique.hxx"