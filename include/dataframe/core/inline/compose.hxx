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

#include <dataframe/core/pipe.h>
#include <functional>

namespace df {

template <typename T> inline auto compose(T &&value) {
    return pipe(std::forward<T>(value));
}

template <typename T, typename F, typename... Rest>
inline auto compose(T &&value, F &&operation, Rest &&...rest) {
    if constexpr (sizeof...(Rest) == 0) {
        return operation(value);
    } else {
        return operation(
            compose(std::forward<T>(value), std::forward<Rest>(rest)...));
    }
}

template <typename F> inline auto make_compose(F &&operation) {
    return make_pipe(std::forward<F>(operation));
}

template <typename F, typename... Rest>
inline auto make_compose(F &&first, Rest &&...rest) {
    return make_pipe(make_pipe(std::forward<Rest>(rest)...),
                     std::forward<F>(first));
}

} // namespace df
