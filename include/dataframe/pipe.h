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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/common.h>
#include <utility>

namespace df {

// Base case: end of pipe chain
template <typename T> auto pipe(T &&value) { return std::forward<T>(value); }

// General case: pipe with operation
template <typename T, typename F, typename... Rest>
auto pipe(T &&value, F &&operation, Rest &&...rest) {
    return pipe(operation(std::forward<T>(value)), std::forward<Rest>(rest)...);
}

// Helper for creating operations with additional arguments
// template <typename F, typename... Args> auto bind(F &&f, Args &&...args) {
//     return [f = std::forward<F>(f),
//             ... args = std::forward<Args>(args)](auto &&value) mutable {
//         return f(std::forward<decltype(value)>(value),
//                  std::forward<Args>(args)...);
//     };
// }

// Operator | overload for pipe operations
template <typename T, typename F> auto operator|(T &&value, F &&operation) {
    return operation(std::forward<T>(value));
}

// Base case: single operation
template <typename F> auto make_pipe(F &&operation) {
    return [op = std::forward<F>(operation)](const auto &value) {
        return op(value);
    };
}

// General case: multiple operations
template <typename F, typename... Rest>
auto make_pipe(F &&first, Rest &&...rest) {
    return [first = std::forward<F>(first),
            rest_pipe = make_pipe(std::forward<Rest>(rest)...)](
               const auto &value) { return rest_pipe(first(value)); };
}

// Helper to compose operations from right to left (like mathematical function
// composition)
template <typename F> auto compose(F &&operation) {
    return make_pipe(std::forward<F>(operation));
}

template <typename F, typename... Rest> auto compose(F &&last, Rest &&...rest) {
    return make_pipe(compose(std::forward<Rest>(rest)...),
                     std::forward<F>(last));
}

} // namespace df