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
#include <dataframe/functional/utils/partition_n.h>
#include <dataframe/utils.h>
#include <future>
#include <thread>
#include <type_traits>

namespace df {
namespace utils {

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
Serie parallel_execute(Functor &&fct, const Serie &serie, uint nbCores) {
    static_assert(details::returns_serie_v<Functor>,
                  "Functor must return a Serie");

    if (nbCores < 1) {
        throw std::invalid_argument("NnCores must be >= 1");
    }

    if (nbCores == 1) {
        return fct(serie);
    }

    // Get ordered partitions
    auto partitions = df::utils::partition_n(nbCores, serie);

    // Store start indices of each partition for verification
    std::vector<uint32_t> starts;
    starts.reserve(partitions.size());
    uint32_t current = 0;
    for (const auto &p : partitions) {
        starts.push_back(current);
        current += p.count();
    }

    // Launch jobs
    std::vector<std::future<Serie>> futures;
    futures.reserve(partitions.size());

    for (size_t i = 0; i < partitions.size(); ++i) {
        futures.push_back(
            std::async(std::launch::async, [fct, partition = partitions[i],
                                            start = starts[i]]() {
                try {
                    auto result = fct(partition);
                    // Verify result size matches input partition
                    if (result.count() != partition.count()) {
                        throw std::runtime_error("Result size mismatch");
                    }
                    return result;
                } catch (const std::exception &e) {
                    throw std::invalid_argument("Job failed: " +
                                                std::string(e.what()));
                }
            }));
    }

    // Collect results in order
    std::vector<Serie> results;
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
        throw std::runtime_error("Total result size mismatch");
    }

    return concat(results);
}

// ------------------------------------
//          A simpler method
// ------------------------------------
/*
template <typename Functor>
Serie parallel_execute(Functor&& fct, const Series& partitions) {
   auto nbJobs = partitions.size();
   
   // Vector to store future results
   std::vector<std::future<Serie>> futures;
   futures.reserve(nbJobs);

   // Launch each job in a separate thread and get future Serie
   for (const auto& job : partitions) {
       futures.push_back(std::async(std::launch::async, [fct, job]() {
           try {
               return fct(job);
           } catch (const std::exception& e) {
               throw std::invalid_argument("error, job failed: " + std::string(e.what()));
           }
       }));
   }

   // Collect results in order and concat them
   std::vector<Serie> results;
   results.reserve(nbJobs);
   
   for (auto& future : futures) {
       results.push_back(future.get());
   }

   return concat(results);
}
*/

} // namespace utils
} // namespace df
