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
#include <dataframe/utils.h>
#include <future>
#include <thread>

namespace df {
namespace utils {

// Helper to run transformation in parallel and collect results
template <typename F>
auto whenAll(F &&transform, const std::vector<Serie> &series) {
    std::vector<std::future<Serie>> futures;
    futures.reserve(series.size());

    // Launch parallel transformations
    for (const auto &s : series) {
        futures.push_back(std::async(
            std::launch::async, [transform, s]() { return transform(s); }));
    }

    // Collect results in order
    std::vector<Serie> results;
    results.reserve(series.size());
    for (auto &f : futures) {
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
    Serie s1(3, {...});
    Serie s2(3, {...});
    Serie s3(3, {...});

    // Parallel execution with transformation
    auto result1 = whenAll([](const Serie &s) { return eigenValues(s); },
                                    {s1, s2, s3});

    // Parallel execution of different Series
    auto [r1, r2, r3] = whenAll(s1, s2, s3);
    ```
 */
template <typename... Series> auto whenAll(const Series &...series) {
    std::vector<std::future<Serie>> futures;
    futures.reserve(sizeof...(series));

    // Launch each Serie in parallel
    (futures.push_back(std::async(
         std::launch::async, [](const Serie &s) { return s.clone(); }, series)),
     ...);

    // Collect results in order
    std::vector<Serie> results;
    results.reserve(sizeof...(series));
    for (auto &f : futures) {
        results.push_back(f.get());
    }

    return std::make_tuple(results...);
}

} // namespace utils
} // namespace df
