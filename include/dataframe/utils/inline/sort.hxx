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

#include <algorithm>
#include <execution>
#include <numeric>

namespace df {

#if HAS_PARALLEL_ALGORITHMS
#else
// Overload sort to ignore the execution policy when parallel support is missing
template <typename Iterator, typename Compare>
void sort(ExecutionPolicyTraits::dummy_policy, Iterator first, Iterator last,
          Compare comp) {
    std::sort(first, last, comp);
}

template <typename Iterator>
void sort(ExecutionPolicyTraits::dummy_policy, Iterator first, Iterator last) {
    std::sort(first, last);
}
#endif

template <typename T> class Sort {
  public:
    // Basic sort with order and execution policy
    static Serie<T> sort(const Serie<T> &serie,
                         SortOrder order = SortOrder::ASCENDING,
                         ExecutionPolicy exec = ExecutionPolicy::SEQ) {
        std::vector<T> result(serie.data());
        auto policy = get_execution_policy(exec);

        if (order == SortOrder::ASCENDING) {
#if HAS_PARALLEL_ALGORITHMS
            std::sort(policy, result.begin(), result.end());
#else
            std::sort(result.begin(), result.end());
#endif
        } else {
#if HAS_PARALLEL_ALGORITHMS
            std::sort(policy, result.begin(), result.end(), std::greater<T>());
#else
            std::sort(result.begin(), result.end(), std::greater<T>());
#endif
        }
        return Serie<T>(result);
    }

    // Sort with custom comparator
    template <typename Compare>
    static Serie<T> sort(const Serie<T> &serie, Compare comp,
                         ExecutionPolicy exec = ExecutionPolicy::SEQ) {
        std::vector<T> result(serie.data());
        auto policy = get_execution_policy(exec);

#if HAS_PARALLEL_ALGORITHMS
        std::sort(policy, result.begin(), result.end(), comp);
#else
        std::sort(result.begin(), result.end(), comp);
#endif

        return Serie<T>(result);
    }

    // Sort by key function
    template <typename KeyFunc>
    static Serie<T> sort_by(const Serie<T> &serie, KeyFunc key_func,
                            SortOrder order = SortOrder::ASCENDING,
                            ExecutionPolicy exec = ExecutionPolicy::SEQ) {
        std::vector<size_t> indices(serie.size());
        std::iota(indices.begin(), indices.end(), 0);
        auto policy = get_execution_policy(exec);

        auto comp = [&](size_t i, size_t j) {
            if (order == SortOrder::ASCENDING) {
                return key_func(serie[i]) < key_func(serie[j]);
            }
            return key_func(serie[i]) > key_func(serie[j]);
        };

#if HAS_PARALLEL_ALGORITHMS
        std::sort(policy, indices.begin(), indices.end(), comp);
#else
        std::sort(indices.begin(), indices.end(), comp);
#endif

        std::vector<T> result;
        result.reserve(serie.size());
        for (size_t idx : indices) {
            result.push_back(serie[idx]);
        }
        return Serie<T>(result);
    }

    // Sort with NaN handling
    static Serie<T> sort_nan(const Serie<T> &serie,
                             SortOrder order = SortOrder::ASCENDING,
                             bool nan_first = false,
                             ExecutionPolicy exec = ExecutionPolicy::SEQ) {
        std::vector<T> result(serie.data());
        auto policy = get_execution_policy(exec);

        auto comp = [order, nan_first](const T &a, const T &b) {
            bool a_nan = std::isnan(a);
            bool b_nan = std::isnan(b);

            if (a_nan && b_nan)
                return false;
            if (a_nan)
                return nan_first;
            if (b_nan)
                return !nan_first;

            return order == SortOrder::ASCENDING ? a < b : a > b;
        };

#if HAS_PARALLEL_ALGORITHMS
        std::sort(policy, result.begin(), result.end(), comp);
#else
        std::sort(result.begin(), result.end(), comp);
#endif

        return Serie<T>(result);
    }
};

// Helper functions with parallel execution support
template <typename T>
Serie<T> sort(const Serie<T> &serie, SortOrder order, ExecutionPolicy exec) {
    return Sort<T>::sort(serie, order, exec);
}

template <typename T, typename Compare>
Serie<T> sort(const Serie<T> &serie, Compare comp, ExecutionPolicy exec) {
    return Sort<T>::sort(serie, comp, exec);
}

template <typename T, typename KeyFunc>
Serie<T> sort_by(const Serie<T> &serie, KeyFunc key_func, SortOrder order,
                 ExecutionPolicy exec) {
    return Sort<T>::sort_by(serie, key_func, order, exec);
}

template <typename T>
Serie<T> sort_nan(const Serie<T> &serie, SortOrder order, bool nan_first,
                  ExecutionPolicy exec) {
    return Sort<T>::sort_nan(serie, order, nan_first, exec);
}

// Bind functions for pipeline operations with parallel support
template <typename T> auto bind_sort(SortOrder order, ExecutionPolicy exec) {
    return [order, exec](const Serie<T> &serie) {
        return sort(serie, order, exec);
    };
}

template <typename Compare>
auto bind_sort_with(Compare comp, ExecutionPolicy exec) {
    return [comp, exec](const auto &serie) { return sort(serie, comp, exec); };
}

template <typename KeyFunc>
auto bind_sort_by(KeyFunc key_func, SortOrder order, ExecutionPolicy exec) {
    return [key_func, order, exec](const auto &serie) {
        return sort_by(serie, key_func, order, exec);
    };
}

} // namespace df
