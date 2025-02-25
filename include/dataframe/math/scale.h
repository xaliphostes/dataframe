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
#include <dataframe/utils/meta.h>
#include <type_traits>

namespace df {

/**
 * @brief Scale any type by an arithmetic scalar value
 */
template <typename T, typename S>
auto scale(const Serie<T> &, const S) -> details::isArithmeticSerie<T, S>;

/**
 * @brief Scale any type by a Serie of arithmetic values
 */
template <typename T, typename S>
auto scale(const Serie<T> &, const Serie<S> &) -> details::isArithmeticSerie<T, S>;

/**
 * @brief Element-wise scaling with the same type
 */
template <typename T>
Serie<T> scale(const Serie<T> &serie, const Serie<T> &scalars);

/**
 * @brief Alternative bind_scale implementation that avoids explicit type
 * specification
 */
struct scale_binder;

/**
 * @brief Helper function to create scale_binder
 */
template <typename S>
auto bind_scale(S scalar)
    -> std::enable_if_t<std::is_arithmetic<S>::value, scale_binder>;

/**
 * @brief Bind function for pipeline operations with any Serie
 */
template <typename T> auto bind_scale(const Serie<T> &scalars);

} // namespace df

#include "inline/scale.hxx"