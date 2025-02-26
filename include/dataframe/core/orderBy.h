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
 * @brief Sort a Serie using a key function to determine order
 *
 * Sorts a collection based on values returned by the provided key function.
 * This provides a convenient way to sort complex objects by a specific property
 * or computed value.
 *
 * @tparam T The type of elements in the Serie
 * @tparam KeyFunc The type of key function (must be callable with T and return
 * a comparable value)
 * @param serie The Serie to sort
 * @param keyFn Function that extracts the sort key from each element
 * @param ascending Whether to sort in ascending (true) or descending (false)
 * order
 * @param exec Execution policy (sequential or parallel)
 * @return Serie<T> A new Serie containing the sorted elements
 *
 * Example usage:
 * @code
 * // Sort a Serie of numbers by their absolute values
 * Serie<double> values{-5, 3, -10, 7, 2};
 * auto sorted = orderBy(values, [](double x) { return std::abs(x); });
 * // Result: [2, 3, -5, 7, -10]
 *
 * // Sort a Serie of custom objects by a specific property
 * struct Person { std::string name; int age; };
 * Serie<Person> people = {...};
 * auto byAge = orderBy(people, [](const Person& p) { return p.age; });
 * auto byName = orderBy(people, [](const Person& p) { return p.name; });
 *
 * // Using with pipe syntax
 * auto result = values | bind_orderBy([](double x) { return std::abs(x); });
 * @endcode
 */
template <typename T, typename KeyFunc>
Serie<T> orderBy(const Serie<T> &serie, KeyFunc keyFn, bool ascending = true,
                 ExecutionPolicy exec = ExecutionPolicy::SEQ);

/**
 * @brief Creates a function object that can be used with pipe operations to
 * sort by key
 *
 * @tparam KeyFunc Type of the key function
 * @param keyFn Function that extracts the sort key from each element
 * @param ascending Whether to sort in ascending (true) or descending (false)
 * order
 * @param exec Execution policy (sequential or parallel)
 * @return A function object compatible with df::pipe
 */
template <typename KeyFunc>
auto bind_orderBy(KeyFunc keyFn, bool ascending = true,
                  ExecutionPolicy exec = ExecutionPolicy::SEQ);

} // namespace df

#include "inline/orderBy.hxx"