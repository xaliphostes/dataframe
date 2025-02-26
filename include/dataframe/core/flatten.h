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

namespace df {

/**
 * @brief Flattens a nested container into a single Serie.
 *
 * This function recursively flattens nested containers (vectors, lists, Series,
 * etc.) into a single Serie. It will extract elements from each level of
 * nesting and combine them all into a flat Serie.
 *
 * @param container The nested container to flatten
 * @return A flattened Serie containing all elements from all levels
 *
 * @code
 * // Flatten a Serie of vectors
 * Serie<std::vector<int>> nested{
 *     {1, 2, 3},
 *     {4, 5},
 *     {6, 7, 8, 9}
 * };
 * auto flat = flatten(nested);
 * // Result: {1, 2, 3, 4, 5, 6, 7, 8, 9}
 *
 * // Flatten a Serie of Series
 * Serie<Serie<double>> series_of_series{
 *     Serie<double>{1.1, 2.2},
 *     Serie<double>{3.3, 4.4, 5.5}
 * };
 * auto flat_series = flatten(series_of_series);
 * // Result: {1.1, 2.2, 3.3, 4.4, 5.5}
 * @endcode
 */
template <typename Container> auto flatten(const Container &container);

/**
 * @brief Specialized version for flattening a Serie of Serie.
 *
 * This is a specialized implementation that handles the common case of a
 * Serie of Series more efficiently.
 *
 * @param series_of_series A Serie containing Series of the same type
 * @return A flattened Serie containing all elements
 */
template <typename T> Serie<T> flatten(const Serie<Serie<T>> &series_of_series);

/**
 * @brief Flattens multiple levels of nested data in a Serie.
 *
 * This function recursively flattens deeply nested data structures up to
 * the specified depth. If depth is not specified, it flattens all levels.
 *
 * @param nested_serie The serie with nested data
 * @param depth The number of levels to flatten (default: INT_MAX = all levels)
 * @return A flattened Serie
 *
 * @code
 * // Deeply nested structure: Serie of vector of vector of int
 * Serie<std::vector<std::vector<int>>> deeply_nested{
 *     {{1, 2}, {3, 4}},
 *     {{5, 6}, {7, 8}}
 * };
 *
 * // Flatten only one level
 * auto partially_flat = flattenDeep(deeply_nested, 1);
 * // Result is a Serie<std::vector<int>>: {{1, 2}, {3, 4}, {5, 6}, {7, 8}}
 *
 * // Flatten all levels
 * auto completely_flat = flattenDeep(deeply_nested);
 * // Result: {1, 2, 3, 4, 5, 6, 7, 8}
 * @endcode
 */
template <typename T>
auto flattenDeep(const Serie<T> &nested_serie, int depth = INT_MAX);

/**
 * @brief Helper function to create a bound flatten operation for use in pipe
 * operations.
 *
 * @return A function that flattens a Serie
 *
 * @code
 * // Using with pipe syntax
 * auto flat = nested | bind_flatten();
 * @endcode
 */
auto bind_flatten();

/**
 * @brief Helper function to create a bound flattenDeep operation for use in
 * pipe operations.
 *
 * @param depth The number of levels to flatten (default: INT_MAX = all levels)
 * @return A function that deeply flattens a Serie
 *
 * @code
 * // Using with pipe syntax to flatten one level
 * auto partially_flat = deeply_nested | bind_flattenDeep(1);
 *
 * // Using with pipe syntax to flatten all levels
 * auto completely_flat = deeply_nested | bind_flattenDeep();
 * @endcode
 */
auto bind_flattenDeep(int depth = INT_MAX);

} // namespace df

#include "inline/flatten.hxx"
