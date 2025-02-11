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
#include "execution_policy.h"
#include <dataframe/Serie.h>

/**
 * @brief Sort a Serie in ascending or descending order
 *
 * Key features of this implementation:
 * - Supports both ascending and descending sort
 * - Allows custom comparison functions
 * - Maintains immutability (returns new Serie instead of modifying in place)
 * - Provides binding for pipeline operations
 * - Handles edge cases (empty series, single element)
 *
 * @code
 * // Basic sorting
 * Serie<double> s1{5, 2, 8, 1, 9};
 * auto sorted1 = sort(s1);                           // [1, 2, 5, 8, 9]
 * auto sorted2 = sort(s1, SortOrder::DESCENDING);    // [9, 8, 5, 2, 1]
 *
 * // Custom comparator
 * auto sorted3 = sort(s1, [](double a, double b) {
 *     return std::abs(a) < std::abs(b);
 * });
 *
 * // Sort by key function
 * struct Person { std::string name; int age; };
 * Serie<Person> people = {...};
 * auto by_age = sort_by(people, [](const Person& p) { return p.age; });
 * auto by_name = sort_by(people, [](const Person& p) { return p.name; });
 *
 * // Sort with NaN handling
 * Serie<double> s2{5, std::nan(""), 2, std::nan(""), 1};
 * auto sorted4 = sort_nan(s2);                    // [1, 2, 5, NaN, NaN]
 * auto sorted5 = sort_nan(s2, SortOrder::ASCENDING, true);  // [NaN, NaN, 1, 2,
 * 5]
 *
 * // Pipeline usage
 * auto result = s1
 *     | bind_sort<double>(SortOrder::ASCENDING)
 *     | bind_map([](double x) { return x * 2; });
 * @endcode
 */
namespace df {

enum class SortOrder { ASCENDING, DESCENDING };

template <typename T>
Serie<T> sort(const Serie<T> &serie, SortOrder order = SortOrder::ASCENDING,
              ExecutionPolicy exec = ExecutionPolicy::SEQ);

template <typename T, typename Compare>
Serie<T> sort(const Serie<T> &serie, Compare comp,
              ExecutionPolicy exec = ExecutionPolicy::SEQ);

template <typename T, typename KeyFunc>
Serie<T> sort_by(const Serie<T> &serie, KeyFunc key_func,
                 SortOrder order = SortOrder::ASCENDING,
                 ExecutionPolicy exec = ExecutionPolicy::SEQ);

template <typename T>
Serie<T> sort_nan(const Serie<T> &serie, SortOrder order = SortOrder::ASCENDING,
                  bool nan_first = false,
                  ExecutionPolicy exec = ExecutionPolicy::SEQ);

// Bind functions for pipeline operations with parallel support
template <typename T>
auto bind_sort(SortOrder order = SortOrder::ASCENDING,
               ExecutionPolicy exec = ExecutionPolicy::SEQ);

template <typename Compare>
auto bind_sort_with(Compare comp, ExecutionPolicy exec = ExecutionPolicy::SEQ);

template <typename KeyFunc>
auto bind_sort_by(KeyFunc key_func, SortOrder order = SortOrder::ASCENDING,
                  ExecutionPolicy exec = ExecutionPolicy::SEQ);

} // namespace df

#include "inline/sort.hxx"