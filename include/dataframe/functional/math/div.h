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
#include <dataframe/functional/map.h>

namespace df {
namespace math {

template <typename T>
std::enable_if_t<df::details::is_floating_v<T>, GenSerie<T>>
div(const GenSerie<T> &serie, T d);

template <typename T>
std::enable_if_t<df::details::is_floating_v<T>, GenSerie<T>>
div(const GenSerie<T> &serie, const GenSerie<T> &divider);

template <typename T>
inline std::enable_if_t<df::details::is_floating_v<T>,
                        std::function<GenSerie<T>(const GenSerie<T> &)>>
make_div(T scalar) {
    return [scalar](const GenSerie<T> &serie) -> GenSerie<T> {
        return div(serie, scalar);
    };
}

template <typename T>
inline std::enable_if_t<df::details::is_floating_v<T>,
                        std::function<GenSerie<T>(const GenSerie<T> &)>>
make_div(const GenSerie<T> &divider) {
    return [&divider](const GenSerie<T> &serie) -> GenSerie<T> {
        return div(serie, divider);
    };
}

} // namespace math
} // namespace df

#include "inline/div.hxx"