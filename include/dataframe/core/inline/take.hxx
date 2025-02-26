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

#include <stdexcept>
#include <tuple>
#include <vector>

namespace df {

namespace detail {

// Helper to take first n elements from a series
template <typename T> Serie<T> take_elements(const Serie<T> &serie, size_t n) {
    size_t actual_count = std::min(n, serie.size());

    std::vector<T> result;
    result.reserve(actual_count);

    for (size_t i = 0; i < actual_count; ++i) {
        result.push_back(serie[i]);
    }

    return Serie<T>(result);
}

} // namespace detail

// ----------------------------------------------------------

// Take first n elements from a single series
template <typename T> inline Serie<T> take(const Serie<T> &serie, size_t n) {
    return detail::take_elements(serie, n);
}

// Take first n elements from multiple series
template <typename T, typename... Ts>
inline std::tuple<Serie<T>, Serie<Ts>...> take(size_t n, const Serie<T> &first,
                                               const Serie<Ts> &...rest) {
    // Ensure all series have the same size
    const size_t size = first.size();
    if (!((rest.size() == size) && ...)) {
        throw std::runtime_error(
            "All series must have the same size for take operation");
    }

    return std::make_tuple(detail::take_elements(first, n),
                           detail::take_elements(rest, n)...);
}

// Helper for binding take to use in pipe operations
template <typename T> inline auto bind_take(size_t n) {
    return [n](const Serie<T> &serie) { return take(serie, n); };
}

} // namespace df
