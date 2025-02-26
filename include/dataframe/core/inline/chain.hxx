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

#include <vector>

namespace df {

namespace detail {

// Helper to calculate the total size needed for the result
template <typename T, typename... Args>
size_t total_chain_size(const Serie<T> &first, const Args &...rest) {
    return first.size() + (... + rest.size());
}

// Helper to append elements from a Serie to a vector
template <typename T>
void append_to_vector(std::vector<T> &result, const Serie<T> &serie) {
    for (size_t i = 0; i < serie.size(); ++i) {
        result.push_back(serie[i]);
    }
}

} // namespace detail

// Binary version - chain two Series
template <typename T>
Serie<T> chain(const Serie<T> &first, const Serie<T> &second) {
    if (first.empty()) {
        return second;
    }
    if (second.empty()) {
        return first;
    }

    std::vector<T> result;
    result.reserve(first.size() + second.size());

    detail::append_to_vector(result, first);
    detail::append_to_vector(result, second);

    return Serie<T>(result);
}

// Variadic version - chain multiple Series
template <typename T, typename... Args>
Serie<T> chain(const Serie<T> &first, const Args &...rest) {
    // Handle special case for just one Serie
    if constexpr (sizeof...(rest) == 0) {
        return first;
    }
    // Handle binary case directly
    else if constexpr (sizeof...(rest) == 1) {
        return chain(first, std::get<0>(std::tuple<const Args &...>(rest...)));
    }
    // General case with more than two Series
    else {
        std::vector<T> result;
        result.reserve(detail::total_chain_size(first, rest...));

        detail::append_to_vector(result, first);
        (detail::append_to_vector(result, rest), ...);

        return Serie<T>(result);
    }
}

// Bind function for pipe operations
template <typename... Args> auto bind_chain(const Args &...rest) {
    return [&rest...](const auto &first) { return chain(first, rest...); };
}

} // namespace df
