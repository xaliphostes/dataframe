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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/types.h>
#include <dataframe/utils/concat.h>
#include <future>
#include <thread>

namespace df {
namespace utils {

    // Helper to create tuple from results
template<typename T, std::size_t N, std::size_t... I>
auto make_tuple_from_results_impl(std::vector<std::future<Serie<T>>>& futures, std::index_sequence<I...>) {
    return std::make_tuple((futures[I].get())...);
}

template<typename T, std::size_t N>
auto make_tuple_from_results(std::vector<std::future<Serie<T>>>& futures) {
    return make_tuple_from_results_impl<T, N>(futures, std::make_index_sequence<N>{});
}

/**
 * @brief Helper to run transformation in parallel and collect results
 * @ingroup Utils
 */
template <typename T, typename F>
auto whenAll(F&& transform, const std::vector<Serie<T>>& series) {
    std::vector<std::future<Serie<T>>> futures;
    futures.reserve(series.size());

    // Launch parallel transformations
    for (const auto& s : series) {
        futures.push_back(std::async(
            std::launch::async, [transform, s]() { return transform(s); }));
    }

    // Collect results in order
    std::vector<Serie<T>> results;
    results.reserve(series.size());
    for (auto& f : futures) {
        results.push_back(f.get());
    }

    return concat(results);
}

/**
 * Parallel execution of multiple Series
 *
 * Key features:
 * - Parallel execution of transformations
 * - Preserves order of results
 * - Works with both same and different itemSize Series
 * - Error handling through futures
 * - Resource management through RAII
 * @example
 * ```cpp
 * // Usage:
    Serie<double> s1(3, {...});
    Serie<double> s2(3, {...});
    Serie<double> s3(3, {...});

    // Parallel execution with transformation
    auto result1 = whenAll([](const Serie<double>& s) { return
        eigenValues(s); }, {s1, s2, s3});

    // Parallel execution of different Series
    auto [r1, r2, r3] = whenAll(s1, s2, s3);
    ```
    @ingroup Utils
 */
template <typename T, typename... Series>
auto whenAll(const Series&... series) {
    static_assert((std::is_same_v<Series, Serie<T>> && ...),
                 "All series must be of the same type Serie<T>");

    std::vector<std::future<Serie<T>>> futures;
    futures.reserve(sizeof...(series));

    // Launch each Serie in parallel
    (futures.push_back(std::async(
         std::launch::async, [](const Serie<T>& s) { return s.clone(); }, series)),
     ...);

    // Create a tuple of indices for expansion
    return make_tuple_from_results<T, sizeof...(Series)>(futures);
}

} // namespace utils
} // namespace df
