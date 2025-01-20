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
#include <dataframe/functional/geo/gradient.h>
#include <dataframe/functional/map.h>

namespace df {
namespace geo {

/**
 * @brief Compute strain tensor from displacement field
 * @param u Displacement field (itemSize=3)
 * @param grid_size Grid dimensions {nx,ny,nz}
 * @param dx Grid spacing {dx,dy,dz}
 * @return GenSerie<T> with itemSize=6 containing strain tensor components
 *         [εxx, εyy, εzz, εxy, εyz, εxz]
 */
template <typename T>
GenSerie<T> strain(const GenSerie<T> &u, const std::vector<uint32_t> &grid_size,
                   const std::vector<T> &dx);

/**
 * @brief Create strain operation that can be used with the pipe operator
 * @param grid_size Grid dimensions {nx,ny,nz}
 * @param dx Grid spacing {dx,dy,dz}
 */
template <typename T>
auto make_strain(const std::vector<uint32_t> &, const std::vector<T> &);

} // namespace geo
} // namespace df

#include "inline/strain.hxx"