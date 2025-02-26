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
 */

#pragma once
#include <dataframe/Serie.h>
#include <functional>
#include <map>
#include <tuple>

namespace df {

/**
 * @brief Groups a series by a key function.
 *
 * This function divides a series into groups based on keys generated by
 * applying the key function to each element. Elements with the same key are
 * grouped together.
 *
 * @param serie The series to group
 * @param key_func Function that takes a value and its index and returns a key
 * @return A map where keys are the result of the key function, and values are
 * series containing the matching elements
 *
 * @code
 * // Group numbers by their remainder when divided by 3
 * Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9};
 * auto grouped = groupBy(numbers, [](int n, size_t) { return n % 3; });
 *
 * // Result:
 * // grouped[0] = {3, 6, 9}
 * // grouped[1] = {1, 4, 7}
 * // grouped[2] = {2, 5, 8}
 * @endcode
 */
template <typename T, typename KeyFunc>
auto groupBy(const Serie<T> &serie, KeyFunc &&key_func);

/**
 * @brief Groups multiple series based on a key function applied to the first
 * series.
 *
 * This function divides multiple series into groups based on keys generated by
 * applying the key function to elements of the first series. Corresponding
 * elements from all series with the same key are grouped together.
 *
 * @param first The first series to group (and the one to apply the key function
 * to)
 * @param key_func Function that takes a value from the first series and its
 * index, and returns a key
 * @param rest Additional series to group in parallel with the first
 * @return A map where keys are the result of the key function, and values are
 * tuples of series containing the matching elements
 *
 * @code
 * // Group people by age category
 * Serie<int> ages{25, 18, 35, 62, 17, 42};
 * Serie<std::string> names{"Alice", "Bob", "Charlie", "Dave", "Eve", "Frank"};
 *
 * auto grouped = groupBy(ages,
 *     [](int age, size_t) {
 *         if (age < 18) return "minor";
 *         if (age < 30) return "young_adult";
 *         if (age < 60) return "adult";
 *         return "senior";
 *     },
 *     names
 * );
 *
 * // Result:
 * // grouped["minor"] = ({17}, {"Eve"})
 * // grouped["young_adult"] = ({25, 18}, {"Alice", "Bob"})
 * // grouped["adult"] = ({35, 42}, {"Charlie", "Frank"})
 * // grouped["senior"] = ({62}, {"Dave"})
 * @endcode
 */
template <typename T, typename KeyFunc, typename... Ts>
auto groupBy(const Serie<T> &first, KeyFunc &&key_func,
             const Serie<Ts> &...rest);

/**
 * @brief Groups a series by a boolean predicate.
 *
 * This function divides a series into two groups: elements that satisfy the
 * predicate (true) and elements that don't (false).
 *
 * @param serie The series to group
 * @param pred Function that takes a value and returns a boolean
 * @return A map with boolean keys where true maps to elements satisfying the
 * predicate, and false maps to elements that don't
 *
 * @code
 * // Separate even and odd numbers
 * Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9};
 * auto grouped = groupByPredicate(numbers, [](int n) { return n % 2 == 0; });
 *
 * // Result:
 * // grouped[true] = {2, 4, 6, 8}  // even numbers
 * // grouped[false] = {1, 3, 5, 7, 9}  // odd numbers
 * @endcode
 */
template <typename T, typename Predicate>
auto groupByPredicate(const Serie<T> &serie, Predicate &&pred);

/**
 * @brief Helper function to create a bound groupBy operation for use in pipe
 * operations.
 *
 * @param key_func Function that takes a value and its index and returns a key
 * @return A function that groups a series using the provided key function
 *
 * @code
 * // Using with pipe syntax
 * auto grouped = numbers | bind_groupBy([](int n, size_t) { return n % 3; });
 * @endcode
 */
template <typename KeyFunc> auto bind_groupBy(KeyFunc &&key_func);

/**
 * @brief Helper function to create a bound groupByPredicate operation for use
 * in pipe operations.
 *
 * @param pred Function that takes a value and returns a boolean
 * @return A function that groups a series using the provided predicate
 *
 * @code
 * // Using with pipe syntax
 * auto grouped = numbers | bind_groupByPredicate([](int n) { return n % 2 == 0;
 * });
 * @endcode
 */
template <typename Predicate> auto bind_groupByPredicate(Predicate &&pred);

} // namespace df

#include "inline/groupBy.hxx"
