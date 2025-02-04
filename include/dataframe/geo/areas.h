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
#include <dataframe/utils.h>
#include <stdexcept>

namespace df {

/**
 * Compute areas of a series of triangles in 3D
 * @param vertices Serie of 3D vertices
 * @param triangles Serie of index triplets defining triangles
 * @return Serie of triangle areas
 */
template <typename T>
Serie<T> area(const Serie<Vector3> &vertices, const Serie<iVector3> &triangles);

/**
 * Compute areas of a series of triangles in 2D
 * @param vertices Serie of 2D vertices
 * @param triangles Serie of index triplets defining triangles
 * @return Serie of triangle areas
 */
template <typename T>
Serie<T> area(const Serie<Vector2> &vertices, const Serie<iVector3> &triangles);

// Binding functions for pipeline operations
template <typename T> auto bind_area(const Serie<iVector3> &triangles);

} // namespace df

#include "inline/areas.hxx"