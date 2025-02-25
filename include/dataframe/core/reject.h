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
#include <dataframe/utils/utils.h>
#include <vector>

/**
 * @brief Function that filters out elements from a Serie based on a predicate
 * condition.
 *
 * Key features:
 * - Works with any Serie type (scalar, vector, matrix)
 * - Supports multi-series operations
 * - Predefined predicates for common operations
 * - Pipeline integration
 * - Memory efficient (pre-reserves vector space)
 * - Error checking for size mismatches
 *
 * @code
 * // Basic usage
 * auto result = reject([](double x) { return x < 0; }, serie);
 *
 * // Using predefined predicates
 * auto result = reject(outside(0.0, 1.0), serie);
 *
 * // Pipeline usage
 * auto result = serie | bind_reject(less_than(0.0));
 *
 * // Multi-series
 * auto result = reject([](double x, double y) { return x > y; }, serie1,
 * serie2);
 * @endcode
 */

namespace df {

/**
 * Reject elements from a Serie based on a predicate
 * Returns a new Serie containing only elements for which the predicate returns
 * false
 *
 * @param predicate Function that returns true for elements to be rejected
 * @param serie Input Serie
 * @return New Serie with rejected elements removed
 */
template <typename F, typename T>
Serie<T> reject(F &&predicate, const Serie<T> &serie);

/**
 * Multi-series version that applies predicate to corresponding elements
 */
template <typename F, typename T, typename... Args>
Serie<T> reject(F &&predicate, const Serie<T> &first, const Serie<T> &second,
                const Args &...args);

/**
 * Create a predicate for rejecting values less than a threshold
 */
template <typename T> auto less_than(T threshold);

/**
 * Create a predicate for rejecting values greater than a threshold
 */
template <typename T> auto greater_than(T threshold);

/**
 * Create a predicate for rejecting values outside a range [min, max]
 */
template <typename T> auto outside(T min_val, T max_val);

/**
 * Create a predicate for rejecting values equal to a specific value
 */
template <typename T> auto equal_to(T value);

/**
 * Enable pipe operation for reject
 */
template <typename F> auto bind_reject(F &&predicate);

} // namespace df

#include "inline/reject.hxx"