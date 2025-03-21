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

#include <dataframe/utils/meta.h>
#include <dataframe/utils/utils.h>
#include <numeric>
#include <stdexcept>
#include <type_traits>

namespace df {
namespace stats {

namespace detail {

// Helper for scalar types
template <typename T,
          typename std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
inline T compute_window_avg(const std::vector<T> &values, size_t start,
                            size_t count) {
    T sum = T{};
    for (size_t i = 0; i < count; ++i) {
        sum += values[start + i];
    }
    return sum / static_cast<T>(count);
}

// Helper for array/vector types using std::array
template <typename T,
          typename std::enable_if_t<details::is_std_array_v<T>, int> = 0>
inline T compute_window_avg(const std::vector<T> &values, size_t start,
                            size_t count) {
    using ElementType = typename T::value_type;
    constexpr size_t N = std::tuple_size_v<T>;

    // Initialize result array with zeros
    T result{};
    for (size_t j = 0; j < N; ++j) {
        result[j] = ElementType{};
    }

    // Sum the window elements
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result[j] += values[start + i][j];
        }
    }

    // Divide by count
    for (size_t j = 0; j < N; ++j) {
        result[j] /= static_cast<ElementType>(count);
    }

    return result;
}

// Generic sliding window implementation
template <typename T>
inline Serie<T> sliding_window_avg(const Serie<T> &serie, size_t window_size) {
    if (serie.empty()) {
        throw std::runtime_error(
            "Cannot compute moving average on empty serie");
    }

    if (window_size == 0) {
        throw std::runtime_error("Window size must be greater than zero");
    }

    // Special case for window size 1
    if (window_size == 1) {
        return serie; // Return a copy of the input serie
    }

    const size_t n = serie.size();
    std::vector<T> result(n);
    const auto &values = serie.data();

    // Process each position
    for (size_t i = 0; i < n; ++i) {
        // Calculate window bounds
        size_t window_start =
            (i >= window_size - 1) ? i - (window_size - 1) : 0;
        size_t window_count = i - window_start + 1;

        // Compute average for this window
        result[i] = compute_window_avg(values, window_start, window_count);
    }

    return Serie<T>(result);
}

} // namespace detail

template <typename T>
inline Serie<T> moving_avg(const Serie<T> &serie, size_t window_size) {
    return detail::sliding_window_avg(serie, window_size);
}

template <typename T> inline auto bind_moving_avg(size_t window_size) {
    return [window_size](const Serie<T> &serie) {
        return moving_avg(serie, window_size);
    };
}

} // namespace stats
} // namespace df