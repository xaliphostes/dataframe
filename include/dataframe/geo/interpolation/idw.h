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
#include "common.h"
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Inverse Distance Weighting (idw) interpolation for 2D or 3D points
 * @param points Serie of 2D/3D points (Vector2/Vector3)
 * @param values Serie of values at each point
 * @param targets Serie of points to interpolate to
 * @param power Power parameter for IDW (typically 2)
 * @param smoothing Smoothing factor to prevent division by zero
 * @return Serie of interpolated values
 * 
 * @code
 * // Create sample data
 * df::Serie<df::Vector2> points = {
 *     {0,0}, {1,0}, {0,1}, {1,1}
 * };
 * df::Serie<double> values = {0, 1, 1, 2};
 * 
 * // Points to interpolate to
 * df::Serie<df::Vector2> targets = {
 *     {0.5, 0.5}, {0.25, 0.75}
 * };
 * 
 * // Interpolate using IDW
 * auto interpolated = df:::interpolate_idw<2>(points, values, targets);
 * @endcode
 */
template <typename T, size_t DIM>
Serie<T> idw(const Serie<std::array<T, DIM>> &points, const Serie<T> &values,
             const Serie<std::array<T, DIM>> &targets, double power = 2.0,
             double smoothing = 1e-10) {
    return detail::idw_traits<T, DIM>::idw(points, values, targets, power,
                                           smoothing);
}

} // namespace df