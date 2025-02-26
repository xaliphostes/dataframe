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
 * @brief Divides a series into chunks of specified size.
 *
 * This function splits a series into multiple chunks where each chunk has
 * the specified size, except possibly the last chunk which may be smaller
 * if the series size is not evenly divisible by the chunk size.
 *
 * @param chunk_size Size of each chunk
 * @param serie The series to chunk
 * @return std::vector<Serie<T>> Vector of series chunks
 *
 * @code
 * // Create a series of values
 * Serie<int> values{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
 *
 * // Chunk into pieces of size 3
 * auto chunks = chunk(3, values);
 * // Result: [{1,2,3}, {4,5,6}, {7,8,9}, {10}]
 *
 * // Using with pipe syntax
 * auto chunks_pipe = values | bind_chunk<int>(3);
 * @endcode
 */
template <typename T>
std::vector<Serie<T>> chunk(size_t chunk_size, const Serie<T> &serie);

/**
 * @brief Divides multiple series into corresponding chunks of specified size.
 *
 * This function splits multiple series into chunks where each chunk has
 * the specified size. All input series must have the same length.
 *
 * @param chunk_size Size of each chunk
 * @param first The first series to chunk
 * @param rest Additional series to chunk
 * @return Vector of tuples, where each tuple contains corresponding chunks from
 * all series
 *
 * @code
 * // Create series of values
 * Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8};
 * Serie<std::string> labels{"a", "b", "c", "d", "e", "f", "g", "h"};
 *
 * // Chunk both series with size 3
 * auto chunks = chunk(3, numbers, labels);
 * // Result is a vector of tuples:
 * // [({1,2,3}, {"a","b","c"}), ({4,5,6}, {"d","e","f"}), ({7,8}, {"g","h"})]
 * @endcode
 */
template <typename T, typename... Ts>
auto chunk(size_t chunk_size, const Serie<T> &first, const Serie<Ts> &...rest);

/**
 * @brief Helper function to create a bound chunk operation for use in pipe
 * operations.
 *
 * @param chunk_size Size of each chunk
 * @return A function that chunks a series into pieces of the specified size
 */
template <typename T> auto bind_chunk(size_t chunk_size);

} // namespace df

#include "inline/chunk.hxx"
