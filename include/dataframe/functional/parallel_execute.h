/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <type_traits>

namespace df {

namespace details {

// Helper to detect return type of functor when called with a Serie
template <typename F> struct returns_serie {
  private:
    template <typename G>
    static auto test(int) -> decltype(std::is_same<decltype(std::declval<G>()(
                                                       std::declval<Serie>())),
                                                   Serie>{});

    template <typename> static std::false_type test(...);

  public:
    static constexpr bool value = decltype(test<F>(0))::value;
};

template <typename F>
inline constexpr bool returns_serie_v = returns_serie<F>::value;

} // namespace details

// -----------------------------------------------------------

/**
 * Execute in parallel the fct on n = partitions.size() cores.
 * Each Functor should return a Serie. After the parallel computation, all
 * series are concatenated.
 */
template <typename Functor>
Serie parallel_execute(Functor &&fct, const Series &partitions) {
    static_assert(details::returns_serie_v<Functor>,
                  "Functor must return a Serie");

    auto nbJobs = partitions.size();

    // Vector to store future results
    std::vector<std::future<Serie>> futures;
    futures.reserve(nbJobs);

    // Launch each job in a separate thread and get future Serie
    for (const auto &job : partitions) {
        futures.push_back(std::async(std::launch::async, [fct, job]() {
            try {
                return fct(job);
            } catch (const std::exception &e) {
                throw std::invalid_argument("error, job failed: " +
                                            std::string(e.what()));
            }
        }));
    }

    // Collect results in order and concat them
    std::vector<Serie> results;
    results.reserve(nbJobs);

    for (auto &future : futures) {
        results.push_back(future.get());
    }

    return concat(results);
}

} // namespace df
