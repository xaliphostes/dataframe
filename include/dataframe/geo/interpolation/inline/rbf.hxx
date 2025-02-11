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
#include <Eigen/Dense>
#include <cmath>
#include <functional>

namespace df {

template <typename T>
inline Serie<T> rbf_2d(const Serie<Vector2> &points, const Serie<T> &values,
                       const Serie<Vector2> &targets, RBFKernel kernel,
                       double epsilon, double regularization) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    const size_t n = points.size();
    auto kernel_fn = get_kernel_function(kernel);

    // Build the interpolation matrix
    Eigen::MatrixXd A(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double r = std::sqrt(distance_squared_2d(points[i], points[j]));
            A(i, j) = kernel_fn(r, epsilon);
        }
        // Add regularization to diagonal for numerical stability
        A(i, i) += regularization;
    }

    // Convert values to Eigen vector
    Eigen::VectorXd b(n);
    for (size_t i = 0; i < n; ++i) {
        b(i) = static_cast<double>(values[i]);
    }

    // Solve for weights
    Eigen::VectorXd weights = A.ldlt().solve(b);

    // Interpolate at target points
    Serie<T> result(targets.size());
    for (size_t i = 0; i < targets.size(); ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            double r = std::sqrt(distance_squared_2d(targets[i], points[j]));
            sum += weights(j) * kernel_fn(r, epsilon);
        }
        result[i] = static_cast<T>(sum);
    }

    return result;
}

template <typename T>
inline Serie<T> rbf_3d(const Serie<Vector3> &points, const Serie<T> &values,
                       const Serie<Vector3> &targets, RBFKernel kernel,
                       double epsilon, double regularization) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    const size_t n = points.size();
    auto kernel_fn = get_kernel_function(kernel);

    // Build the interpolation matrix
    Eigen::MatrixXd A(n, n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double r = std::sqrt(distance_squared_3d(points[i], points[j]));
            A(i, j) = kernel_fn(r, epsilon);
        }
        // Add regularization to diagonal for numerical stability
        A(i, i) += regularization;
    }

    // Convert values to Eigen vector
    Eigen::VectorXd b(n);
    for (size_t i = 0; i < n; ++i) {
        b(i) = static_cast<double>(values[i]);
    }

    // Solve for weights
    Eigen::VectorXd weights = A.ldlt().solve(b);

    // Interpolate at target points
    Serie<T> result(targets.size());
    for (size_t i = 0; i < targets.size(); ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            double r = std::sqrt(distance_squared_3d(targets[i], points[j]));
            sum += weights(j) * kernel_fn(r, epsilon);
        }
        result[i] = static_cast<T>(sum);
    }

    return result;
}

} // namespace df