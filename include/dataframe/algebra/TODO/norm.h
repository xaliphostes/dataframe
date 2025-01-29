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
#include <dataframe/common.h>
#include <dataframe/map.h>

namespace df {
namespace algebra {

template <typename T> using IsSerieFloating = df::details::IsSerieFloating<T>;

// -----------------------------------------------

/**
 * @ingroup Geo
 */
template <typename T> IsSerieFloating<T> norm2(const Serie<T> &serie);
template <typename T> IsSerieFloating<T> norm(const Serie<T> &serie);

/**
 * Creates a functor for calculating norms
 * Can be used with pipes: serie | make_norm()
 */
template <typename T> auto make_norm() {
    return [](const Serie<T> &serie) -> IsSerieFloating<T> {
        return norm(serie);
    };
}

// Optional: non-templated version if you want to deduce T from usage
inline auto make_norm() {
    return
        [](const auto &serie) -> decltype(norm(serie)) { return norm(serie); };
}

} // namespace algebra
} // namespace df

#include "inline/norm.hxx"