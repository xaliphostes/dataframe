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
#include <dataframe/functional/common.h>

namespace df {

/**
 * Pipe operator: version with one operation
 */
template <typename T, typename F> auto pipe(const GenSerie<T> &serie, F &&op) {
    return op(serie);
}

/**
 * Pipe operator: version with multiple operations
 */
template <typename T, typename F, typename... Fs>
auto pipe(const GenSerie<T> &serie, F &&first, Fs &&...ops) {
    return pipe(first(serie), std::forward<Fs>(ops)...);
}

/**
 * Pipe operator which allows chaining of operations: serie | func1 | func2
 */
template <typename T, typename F>
auto operator|(const GenSerie<T> &serie, F &&func) -> decltype(func(serie)) {
    return func(serie);
}

/**
 * Make the pipe operator chainable
 */
template <typename... Fs> auto make_pipe(Fs &&...ops) {
    return [... ops = std::forward<Fs>(ops)]<typename T>(const T &input) {
        return pipe(input, ops...);
    };
}

} // namespace df
