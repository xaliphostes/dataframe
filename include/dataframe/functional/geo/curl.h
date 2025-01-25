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
namespace geo {

/**
 * @brief Compute the curl of a vector field
 * For 2D vector fields, returns scalar field (z component)
 * For 3D vector fields, returns 3D vector field
 * @param field Vector field (must have itemSize 2 or 3)
 * @param coordinates Point coordinates (itemSize 2 for 2D, 3 for 3D)
 * @return GenSerie<T> Curl of the vector field
 */
template <typename T>
GenSerie<T> curl(const GenSerie<T> &field, const GenSerie<T> &coordinates);

template <typename T> auto make_curl(const GenSerie<T> &coordinates);

} // namespace geo
} // namespace df

#include "inline/curl.hxx"