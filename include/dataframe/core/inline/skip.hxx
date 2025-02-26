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

// Helper to skip first n elements from a series
template <typename T> Serie<T> skip_elements(const Serie<T> &serie, size_t n) {
    if (n >= serie.size()) {
        return Serie<T>(); // Return empty series if we skip all elements
    }

    size_t count = serie.size() - n;
    std::vector<T> result;
    result.reserve(count);

    for (size_t i = n; i < serie.size(); ++i) {
        result.push_back(serie[i]);
    }

    return Serie<T>(result);
}

} // namespace detail

// ----------------------------------------------------------

// Skip first n elements from a single series
template <typename T> inline Serie<T> skip(const Serie<T> &serie, size_t n) {
    return detail::skip_elements(serie, n);
}

// Skip first n elements from multiple series
template <typename T, typename... Ts>
inline std::tuple<Serie<T>, Serie<Ts>...> skip(size_t n, const Serie<T> &first,
                                               const Serie<Ts> &...rest) {
    // Ensure all series have the same size
    const size_t size = first.size();
    if (!((rest.size() == size) && ...)) {
        throw std::runtime_error(
            "All series must have the same size for skip operation");
    }

    return std::make_tuple(detail::skip_elements(first, n),
                           detail::skip_elements(rest, n)...);
}

// Helper for binding skip to use in pipe operations
template <typename T> inline auto bind_skip(size_t n) {
    return [n](const Serie<T> &serie) { return skip(serie, n); };
}

} // namespace df
