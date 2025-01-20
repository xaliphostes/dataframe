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
#include <dataframe/functional/map.h>

namespace df {
namespace utils {

/**
 * Synonym for map
 * @ingroup Utils
 */
// Single série version
template <typename F, typename T>
auto apply(F &&callback, const GenSerie<T> &serie) {
    return df::map(std::forward<F>(callback), serie);
}

// Multi séries version
template <typename F, typename T, typename... Args>
auto apply(F &&callback, const GenSerie<T> &first, const Args &...args) {
    return df::map(std::forward<F>(callback), first, args...);
}

// Make operator version (comme MAKE_OP(map))
template <typename F> auto make_apply(F &&callback) {
    return df::make_map(std::forward<F>(callback));
}

} // namespace utils
} // namespace df
