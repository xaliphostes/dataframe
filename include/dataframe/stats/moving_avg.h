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
namespace stats {

/**
 * @brief Compute moving average of a Serie with specified window size
 *
 * Calculates the average over a sliding window of specified size. For each
 * position in the serie, calculates the average of the current value and the
 * previous (window_size - 1) values. At the beginning of the serie, where there
 * are fewer than window_size previous values, calculates the average of
 * available values.
 *
 * Works with scalar types and array types (vectors, matrices).
 *
 * @param serie Input Serie
 * @param window_size Size of the sliding window
 * @return Serie<T> A new Serie containing moving averages
 * @throws std::runtime_error if serie is empty or window_size is zero
 *
 * Example:
 * @code
 * // Calculate 3-point moving average
 * df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0};
 * auto ma = df::moving_avg(values, 3);
 * // ma = {1.0, 1.5, 2.0, 3.0, 4.0}
 *
 * // Using in a pipeline
 * auto result = values
 *     | df::bind_map([](double x, size_t) { return x * 2; })
 *     | df::bind_moving_avg(3);
 * @endcode
 */
template <typename T>
Serie<T> moving_avg(const Serie<T> &serie, size_t window_size);

/**
 * @brief Create a moving average operation for pipeline use
 *
 * @param window_size Size of the sliding window
 * @return Function that applies moving average to a Serie
 */
template <typename T> auto bind_moving_avg(size_t window_size);

/**
 * @brief Helper to create bound moving average operation using MAKE_OP macro
 */
MAKE_OP(moving_avg);

} // namespace stats
} // namespace df

#include "inline/moving_avg.hxx"