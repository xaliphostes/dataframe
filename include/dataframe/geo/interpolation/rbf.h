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
#include "rbf_kernels.h"

namespace df {

/**
 * @brief RBF interpolation for 2D points
 * @param points Serie of 2D points (Vector2)
 * @param values Serie of values at each point
 * @param targets Serie of points to interpolate to
 * @param kernel RBF kernel type
 * @param epsilon Shape parameter for the kernel
 * @param regularization Regularization parameter for numerical stability
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
 * // Basic usage with default parameters (Multiquadric kernel)
 * auto interpolated = df::interpolate_rbf_2d(points, values, targets);
 * 
 * // Using Gaussian kernel with custom parameters
 * auto interpolated_gaussian = df::rbf_2d(
 *     points, 
 *     values, 
 *     targets,
 *     df::RBFKernel::Gaussian,
 *     2.0,    // epsilon (shape parameter)
 *     1e-8    // regularization
 * );
 * @endcode
 */
template <typename T>
Serie<T> rbf_2d(const Serie<Vector2> &points, const Serie<T> &values,
                const Serie<Vector2> &targets,
                RBFKernel kernel = RBFKernel::Multiquadric,
                double epsilon = 1.0, double regularization = 1e-10);

/**
 * @brief RBF interpolation for 3D points
 * @param points Serie of 3D points (Vector3)
 * @param values Serie of values at each point
 * @param targets Serie of points to interpolate to
 * @param kernel RBF kernel type
 * @param epsilon Shape parameter for the kernel
 * @param regularization Regularization parameter for numerical stability
 * @return Serie of interpolated values
 */
template <typename T>
Serie<T> rbf_3d(const Serie<Vector3> &points, const Serie<T> &values,
                const Serie<Vector3> &targets,
                RBFKernel kernel = RBFKernel::Multiquadric,
                double epsilon = 1.0, double regularization = 1e-10);

} // namespace df

#include "inline/rbf.hxx"