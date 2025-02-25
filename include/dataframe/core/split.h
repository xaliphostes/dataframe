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
#include <dataframe/core/merge.h>
#include <numeric>
#include <tuple>
#include <vector>

namespace df {

namespace detail {
// Calculate chunk sizes for n splits
inline std::vector<size_t> calculate_chunk_sizes(size_t total_size, size_t n) {
    if (n == 0) {
        throw std::invalid_argument("Number of splits must be greater than 0");
    }
    if (n > total_size) {
        n = total_size; // Adjust n if it's larger than the series size
    }

    std::vector<size_t> chunks(n);
    size_t base_size = total_size / n;
    size_t remainder = total_size % n;

    // Distribute the base size to all chunks
    std::fill(chunks.begin(), chunks.end(), base_size);

    // Distribute the remainder one by one
    for (size_t i = 0; i < remainder; ++i) {
        ++chunks[i];
    }

    return chunks;
}

// Helper to create sub-series from a single series
template <typename T>
std::vector<Serie<T>>
create_sub_series(const Serie<T> &serie,
                  const std::vector<size_t> &chunk_sizes) {
    std::vector<Serie<T>> result;
    result.reserve(chunk_sizes.size());

    size_t start = 0;
    for (size_t chunk_size : chunk_sizes) {
        std::vector<T> chunk;
        chunk.reserve(chunk_size);

        for (size_t i = 0; i < chunk_size; ++i) {
            chunk.push_back(serie[start + i]);
        }

        result.emplace_back(chunk);
        start += chunk_size;
    }

    return result;
}
} // namespace detail

// Split a single series into n approximately equal parts
template <typename T>
std::vector<Serie<T>> split_equal(size_t n, const Serie<T> &serie) {
    auto chunk_sizes = detail::calculate_chunk_sizes(serie.size(), n);
    return detail::create_sub_series(serie, chunk_sizes);
}

// Split multiple series into n approximately equal parts
template <typename T, typename... Ts>
auto split_equal_n(size_t n, const Serie<T> &first, const Serie<Ts> &...rest) {
    using namespace detail;

    // Split the first series to get the chunk sizes
    auto chunk_sizes = calculate_chunk_sizes(first.size(), n);

    // Split each series
    std::tuple<std::vector<Serie<T>>, std::vector<Serie<Ts>>...> splits{
        create_sub_series(first, chunk_sizes),
        create_sub_series(rest, chunk_sizes)...};

    // Merge corresponding chunks
    std::vector<std::tuple<Serie<T>, Serie<Ts>...>> result;
    result.reserve(chunk_sizes.size());

    for (size_t i = 0; i < chunk_sizes.size(); ++i) {
        result.emplace_back(std::get<std::vector<Serie<T>>>(splits)[i],
                            std::get<std::vector<Serie<Ts>>>(splits)[i]...);
    }

    return result;
}

// Helper function to create a bound split operation
template <typename T> auto bind_split_equal(size_t n) {
    return [n](const Serie<T> &serie) { return split_equal(n, serie); };
}

} // namespace df
