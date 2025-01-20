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

// Multiplication par un scalaire
template <typename T>
std::enable_if_t<df::details::is_floating_v<T>, GenSerie<T>>
mult(const GenSerie<T> &serie, T d) ;

template <typename T>
std::enable_if_t<df::details::is_floating_v<T>, GenSerie<T>>
mult(const GenSerie<T> &serie, const GenSerie<T> &multiplier) ;

template <typename T> auto make_mult(T scalar) ;

template <typename T> auto make_mult(const GenSerie<T> &multiplier) ;

} // namespace math
} // namespace df

#include "inline/mult.hxx"