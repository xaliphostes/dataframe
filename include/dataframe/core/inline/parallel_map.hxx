/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include <future>
#include <thread>
#include <vector>

namespace df {

namespace detail {

// Helper to determine chunk size for parallel processing
inline size_t get_chunk_size(size_t total_size, size_t num_threads) {
    return (total_size + num_threads - 1) / num_threads;
}

// Helper to get optimal number of threads based on data size
inline size_t get_optimal_threads(size_t data_size) {
    const size_t min_items_per_thread =
        1000; // Adjust based on performance testing
    const size_t available_threads = std::thread::hardware_concurrency();
    const size_t max_useful_threads =
        (data_size + min_items_per_thread - 1) / min_items_per_thread;
    return std::min(available_threads, max_useful_threads);
}

// Process a chunk of data for single series
template <typename F, typename T, typename ResultType>
void process_chunk_single(F &&callback, const Serie<T> &serie,
                          std::vector<ResultType> &result, size_t start,
                          size_t end) {
    for (size_t i = start; i < end && i < serie.size(); ++i) {
        result[i] = callback(serie[i], i);
    }
}

// Process a chunk of data for multiple series
template <typename F, typename T, typename ResultType, typename... Args>
void process_chunk_multi(F &&callback, const Serie<T> &first,
                         const Serie<T> &second,
                         const std::tuple<const Args &...> &args,
                         std::vector<ResultType> &result, size_t start,
                         size_t end) {
    for (size_t i = start; i < end && i < first.size(); ++i) {
        result[i] = std::apply(
            [&](const auto &...rest) {
                return callback(first[i], second[i], (rest[i])..., i);
            },
            args);
    }
}

} // namespace detail

// Single series parallel map
template <typename F, typename T>
inline auto parallel_map(F &&callback, const Serie<T> &serie) {
    using ResultType = decltype(callback(serie[0], 0));
    std::vector<ResultType> result(serie.size());

    if (serie.size() < 1000) { // For small series, use regular map
        return serie.map(std::forward<F>(callback));
    }

    const size_t num_threads = detail::get_optimal_threads(serie.size());
    const size_t chunk_size = detail::get_chunk_size(serie.size(), num_threads);
    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = std::min(start + chunk_size, serie.size());

        futures.push_back(std::async(
            std::launch::async, detail::process_chunk_single<F, T, ResultType>,
            std::ref(callback), std::ref(serie), std::ref(result), start, end));
    }

    // Wait for all threads to complete
    for (auto &future : futures) {
        future.wait();
    }

    return Serie<ResultType>(result);
}

// Multi series parallel map
template <typename F, typename T, typename... Args>
inline auto parallel_map(F &&callback, const Serie<T> &first,
                         const Serie<T> &second, const Args &...args) {
    using ResultType = decltype(callback(first[0], second[0], (args[0])..., 0));
    std::vector<ResultType> result(first.size());

    if (first.size() < 1000) { // For small series, use regular map
        return map(std::forward<F>(callback), first, second, args...);
    }

    const size_t num_threads = detail::get_optimal_threads(first.size());
    const size_t chunk_size = detail::get_chunk_size(first.size(), num_threads);
    std::vector<std::future<void>> futures;

    auto args_tuple = std::make_tuple(std::ref(args)...);

    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = std::min(start + chunk_size, first.size());

        futures.push_back(
            std::async(std::launch::async,
                       detail::process_chunk_multi<F, T, ResultType, Args...>,
                       std::ref(callback), std::ref(first), std::ref(second),
                       std::ref(args_tuple), std::ref(result), start, end));
    }

    // Wait for all threads to complete
    for (auto &future : futures) {
        future.wait();
    }

    return Serie<ResultType>(result);
}

// Bind function for pipe operator
template <typename F> inline auto bind_parallel_map(F &&callback) {
    return [f = std::forward<F>(callback)](const auto &serie) {
        return parallel_map(f, serie);
    };
}

// Alias for shorter name
template <typename F, typename T, typename... Args>
inline auto par_map(F &&callback, const Serie<T> &first, const Args &...args) {
    return parallel_map(std::forward<F>(callback), first, args...);
}

} // namespace df
