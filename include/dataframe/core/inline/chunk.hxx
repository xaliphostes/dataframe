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

#include <dataframe/core/merge.h>
#include <numeric>
#include <tuple>
#include <vector>

namespace df {

namespace detail {

// Calculate number of chunks for the given chunk size
inline size_t calculate_chunk_count(size_t total_size, size_t chunk_size) {
    if (chunk_size == 0) {
        throw std::invalid_argument("Chunk size must be greater than 0");
    }

    // Calculate ceiling division: (total_size + chunk_size - 1) / chunk_size
    return (total_size + chunk_size - 1) / chunk_size;
}

// Helper to create sub-series from a single series with fixed chunk size
template <typename T>
std::vector<Serie<T>> create_fixed_size_chunks(const Serie<T> &serie,
                                               size_t chunk_size) {
    size_t total_size = serie.size();
    size_t num_chunks = calculate_chunk_count(total_size, chunk_size);

    std::vector<Serie<T>> result;
    result.reserve(num_chunks);

    size_t start = 0;
    for (size_t i = 0; i < num_chunks; ++i) {
        // Calculate the actual size of this chunk (might be smaller for the
        // last chunk)
        size_t current_chunk_size = std::min(chunk_size, total_size - start);

        std::vector<T> chunk;
        chunk.reserve(current_chunk_size);

        for (size_t j = 0; j < current_chunk_size; ++j) {
            chunk.push_back(serie[start + j]);
        }

        result.emplace_back(chunk);
        start += current_chunk_size;
    }

    return result;
}

} // namespace detail

// ----------------------------------------------------------

// Chunk a single series into parts of the specified size
template <typename T>
inline std::vector<Serie<T>> chunk(size_t chunk_size, const Serie<T> &serie) {
    return detail::create_fixed_size_chunks(serie, chunk_size);
}

// Chunk multiple series into parts of the specified size
template <typename T, typename... Ts>
inline auto chunk(size_t chunk_size, const Serie<T> &first,
                  const Serie<Ts> &...rest) {
    using namespace detail;

    // Check that all series have the same size
    const size_t size = first.size();
    if (!((rest.size() == size) && ...)) {
        throw std::runtime_error(
            "All series must have the same size for chunk operation");
    }

    // Calculate the number of chunks
    size_t num_chunks = calculate_chunk_count(size, chunk_size);

    // Chunk each series
    std::tuple<std::vector<Serie<T>>, std::vector<Serie<Ts>>...> chunks{
        create_fixed_size_chunks(first, chunk_size),
        create_fixed_size_chunks(rest, chunk_size)...};

    // Combine corresponding chunks from different series
    std::vector<std::tuple<Serie<T>, Serie<Ts>...>> result;
    result.reserve(num_chunks);

    for (size_t i = 0; i < num_chunks; ++i) {
        result.emplace_back(std::get<std::vector<Serie<T>>>(chunks)[i],
                            std::get<std::vector<Serie<Ts>>>(chunks)[i]...);
    }

    return result;
}

// Helper function to create a bound chunk operation
template <typename T> inline auto bind_chunk(size_t chunk_size) {
    return [chunk_size](const Serie<T> &serie) {
        return chunk(chunk_size, serie);
    };
}

} // namespace df
