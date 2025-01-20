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
#include <dataframe/functional/utils/split.h>
// #include <dataframe/functional/utils/zip.h>
#include <dataframe/functional/utils/merge.h>
#include <future>
#include <thread>
#include <type_traits>

namespace df {
namespace details {

// --------------------------------------------------
// Helper to detect return type of functor when called with a GenSerie<T>
// --------------------------------------------------
template <typename F, typename T>
using function_result_t = decltype(std::declval<F>()(std::declval<GenSerie<T> >()));

template <typename F, typename T>
struct returns_same_serie {
    static constexpr bool value = std::is_same<function_result_t<F,T>, GenSerie<T> >::value;
};

template <typename F, typename T>
inline constexpr bool returns_same_serie_v = returns_same_serie<F, T>::value;

} // namespace details
} // namespace df

// -------------------------------------------------------

namespace df {
namespace utils {

/**
 * Execute a function in parallel over partitions of a GenSerie
 * @example
 * ```cpp
 * GenSerie<double> input(3, {...});
 * auto result = par_map([](const GenSerie<double>& s) {
 *     return someOperation(s);
 * }, input, 4); // Use 4 cores
 * ```
 * @ingroup Utils
 */
template <typename F, typename T>
GenSerie<T> par_map(F &&fct, const GenSerie<T> &serie, uint32_t nbCores) {
    static_assert(details::returns_same_serie_v<F, T>,
                  "Function must return GenSerie of same type");

    if (nbCores < 1) {
        throw std::invalid_argument("Number of cores must be >= 1");
    }

    if (nbCores == 1) {
        return fct(serie);
    }

    // Split into partitions
    auto partitions = df::utils::split(nbCores, serie);

    // Store start indices for verification
    std::vector<uint32_t> starts;
    starts.reserve(partitions.size());
    uint32_t current = 0;
    for (const auto &p : partitions) {
        starts.push_back(current);
        current += p.count();
    }

    // Launch parallel jobs
    std::vector<std::future<GenSerie<T>>> futures;
    futures.reserve(partitions.size());

    for (size_t i = 0; i < partitions.size(); ++i) {
        futures.push_back(
            std::async(std::launch::async, [fct, partition = partitions[i],
                                            start = starts[i]]() {
                try {
                    auto result = fct(partition);
                    // Verify result size matches input partition
                    if (result.count() != partition.count()) {
                        throw std::runtime_error(
                            "Result size doesn't match partition size");
                    }
                    if (result.itemSize() != partition.itemSize()) {
                        throw std::runtime_error(
                            "Result itemSize doesn't match partition itemSize");
                    }
                    return result;
                } catch (const std::exception &e) {
                    throw std::runtime_error("Parallel execution failed: " +
                                             std::string(e.what()));
                }
            }));
    }

    // Collect results in order
    std::vector<GenSerie<T>> results;
    results.reserve(partitions.size());

    for (auto &future : futures) {
        results.push_back(future.get());
    }

    // Verify total size
    size_t total_count = 0;
    for (const auto &r : results) {
        total_count += r.count();
    }
    if (total_count != serie.count()) {
        throw std::runtime_error("Total result size doesn't match input size");
    }

    return merge(results); // Using our previously implemented merge
}

// Helper to create a parallel map operation
template <typename F> auto make_par_map(F &&fct, uint32_t nbCores) {
    return [fct = std::forward<F>(fct), nbCores](const auto &serie) {
        using T = typename std::decay_t<decltype(serie)>::value_type;
        return par_map(fct, serie, nbCores);
    };
}

} // namespace utils
} // namespace df
