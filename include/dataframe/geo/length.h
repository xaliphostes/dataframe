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
#include <array>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>
#include <dataframe/utils/utils.h>
#include <stdexcept>

namespace df {

/**
 * Compute lengths of a series of line segments (2D or 3D)
 * @param vertices Serie of vertices
 * @param segments Serie of index pairs defining segments
 * @return Serie of segment lengths
 */
template <size_t N>
Serie<double> length(const Serie<Vector<N>> &vertices,
                     const Segments &segments);

// Binding functions for pipeline operations
template <size_t N> auto bind_length(const Segments &segments);

} // namespace df

#include "inline/lengths.hxx"