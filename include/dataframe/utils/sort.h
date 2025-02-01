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
#include <algorithm>
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Sort a Serie in ascending or descending order
 * 
 * Key features of this implementation:
 * - Supports both ascending and descending sort
 * - Allows custom comparison functions
 * - Maintains immutability (returns new Serie instead of modifying in place)
 * - Provides binding for pipeline operations
 * - Handles edge cases (empty series, single element)
 *
 * @code
 * // Example usage:
 * df::Serie<int> serie{3, 1, 4, 1, 5};
 *
 * // Sort ascending (default)
 * auto result1 = df::sort(serie);  // [1, 1, 3, 4, 5]
 *
 * // Sort descending
 * auto result2 = df::sort(serie, false);  // [5, 4, 3, 1, 1]
 *
 * // Sort with custom comparator
 * auto result3 = df::sort(serie, [](const int& a, const int& b) {
 *     return std::abs(a) < std::abs(b);
 * });
 *
 * // Using bind_sort in a pipeline
 * auto result4 = serie | df::bind_sort(false);  // descending
 * @endcode
 */

// Basic sort with optional ascending/descending flag
template <typename T>
Serie<T> sort(const Serie<T> &serie, bool ascending = true) {
    std::vector<T> sorted_data = serie.data();
    if (ascending) {
        std::sort(sorted_data.begin(), sorted_data.end());
    } else {
        std::sort(sorted_data.begin(), sorted_data.end(), std::greater<T>());
    }
    return Serie<T>(sorted_data);
}

// Sort with custom comparator
template <typename T, typename Compare>
Serie<T> sort(const Serie<T> &serie, Compare comp) {
    std::vector<T> sorted_data = serie.data();
    std::sort(sorted_data.begin(), sorted_data.end(), comp);
    return Serie<T>(sorted_data);
}

// Bind function for pipeline operations
template <typename... Args> auto bind_sort(Args... args) {
    return [args...](const auto &serie) { return sort(serie, args...); };
}

} // namespace df
