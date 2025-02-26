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
#include <tuple>

namespace df {

/**
 * @brief Takes the first n elements from a series.
 *
 * This function returns a new series containing the first n elements from the
 * input series. If n is greater than the size of the series, all elements are
 * returned.
 *
 * @param serie The input series
 * @param n Number of elements to take
 * @return A new series with the first n elements
 *
 * @code
 * // Create a series
 * Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
 *
 * // Take the first 5 elements
 * auto first_five = take(numbers, 5);
 * // Result: {1, 2, 3, 4, 5}
 * @endcode
 */
template <typename T> Serie<T> take(const Serie<T> &serie, size_t n);

/**
 * @brief Takes the first n elements from multiple series.
 *
 * This function returns a tuple of new series, each containing the first n
 * elements from the corresponding input series. All input series must have the
 * same size.
 *
 * @param n Number of elements to take
 * @param first The first input series
 * @param rest Additional input series
 * @return A tuple of new series, each with the first n elements
 *
 * @code
 * // Create series
 * Serie<int> numbers{1, 2, 3, 4, 5};
 * Serie<std::string> names{"Alice", "Bob", "Charlie", "Dave", "Eve"};
 *
 * // Take the first 3 elements from both series
 * auto [first_three_numbers, first_three_names] = take(3, numbers, names);
 * // first_three_numbers: {1, 2, 3}
 * // first_three_names: {"Alice", "Bob", "Charlie"}
 * @endcode
 */
template <typename T, typename... Ts>
std::tuple<Serie<T>, Serie<Ts>...> take(size_t n, const Serie<T> &first,
                                        const Serie<Ts> &...rest);

/**
 * @brief Helper function to create a bound take operation for use in pipe
 * operations.
 *
 * @param n Number of elements to take
 * @return A function that takes the first n elements from a series
 *
 * @code
 * // Using with pipe syntax
 * auto first_five = numbers | bind_take<int>(5);
 * @endcode
 */
template <typename T> auto bind_take(size_t n);

} // namespace df

#include "inline/take.hxx"
