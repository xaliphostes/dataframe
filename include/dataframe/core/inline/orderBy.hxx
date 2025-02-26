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

#include <dataframe/core/execution_policy.h>
#include <algorithm>
#include <numeric>

namespace df {

template <typename T, typename KeyFunc>
inline Serie<T> orderBy(const Serie<T> &serie, KeyFunc keyFn, bool ascending,
                        ExecutionPolicy exec) {
    if (serie.empty()) {
        return serie;
    }

    // Create indices for the sort
    std::vector<size_t> indices(serie.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Get the execution policy
    auto policy = get_execution_policy(exec);

    // Sort indices based on the key function
    auto comparator = [&](size_t i, size_t j) {
        auto key_i = keyFn(serie[i]);
        auto key_j = keyFn(serie[j]);
        return ascending ? (key_i < key_j) : (key_i > key_j);
    };

#if HAS_PARALLEL_ALGORITHMS
    std::sort(policy, indices.begin(), indices.end(), comparator);
#else
    std::sort(indices.begin(), indices.end(), comparator);
#endif

    // Create the sorted result
    std::vector<T> result;
    result.reserve(serie.size());
    for (size_t idx : indices) {
        result.push_back(serie[idx]);
    }

    return Serie<T>(result);
}

/**
 * @brief Creates a function object that can be used with pipe operations to
 * sort by key
 *
 * @tparam KeyFunc Type of the key function
 * @param keyFn Function that extracts the sort key from each element
 * @param ascending Whether to sort in ascending (true) or descending (false)
 * order
 * @param exec Execution policy (sequential or parallel)
 * @return A function object compatible with df::pipe
 */
template <typename KeyFunc>
auto bind_orderBy(KeyFunc keyFn, bool ascending, ExecutionPolicy exec) {
    return [keyFn, ascending, exec](const auto &serie) {
        return orderBy(serie, keyFn, ascending, exec);
    };
}

} // namespace df