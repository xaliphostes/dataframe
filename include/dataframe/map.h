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

namespace df {

// Single series version
template <typename F, typename T>
auto map(F &&callback, const Serie<T> &serie) {
    return serie.map(callback);
}

// Multi series version
template <typename F, typename T, typename... Args>
auto map(F &&callback, const Serie<T> &first, const Serie<T> &second,
         const Args &...args) {
    using ResultType = decltype(callback(first[0], second[0], (args[0])..., 0));
    std::vector<ResultType> result;
    result.reserve(first.size());

    for (size_t i = 0; i < first.size(); ++i) {
        result[i] = callback(first[i], second[i], (args[i])..., i);
    }
    return Serie<ResultType>(result);
}

template <typename F> auto bind_map(F &&callback) {
    return [f = std::forward<F>(callback)](const auto &serie) {
        return map(f, serie);
    };
}

} // namespace df