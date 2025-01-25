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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/functional/common.h>

namespace df {
namespace geo {

/**
 * @brief Compute the curvature of a 3D curve
 * Curvature κ = |r'(t) × r''(t)| / |r'(t)|³
 * where r(t) is the position vector, × is cross product
 * @param points 3D points along the curve (itemSize must be 3)
 * @param parameterization Optional parameter values for the points (default:
 * uniform)
 * @return GenSerie<T> Curvature at each point (scalar field)
 */
template <typename T>
GenSerie<T>
curve_curvature(const GenSerie<T> &points,
                const GenSerie<T> &parameterization = GenSerie<T>()) {
    // Validate input dimensions
    if (points.itemSize() != 3) {
        throw std::invalid_argument("Points must be 3D (itemSize must be 3)");
    }

    const uint32_t count = points.count();
    if (count < 3) {
        throw std::invalid_argument(
            "Need at least 3 points to compute curvature");
    }

    // Create uniform parameterization if none provided
    GenSerie<T> t;
    if (parameterization.isEmpty()) {
        std::vector<T> params(count);
        for (uint32_t i = 0; i < count; ++i) {
            params[i] = static_cast<T>(i) / (count - 1);
        }
        t = GenSerie<T>(1, params);
    } else {
        if (parameterization.count() != count) {
            throw std::invalid_argument(
                "Parameterization must have same count as points");
        }
        t = parameterization;
    }

    GenSerie<T> result(1, count); // Scalar field output for curvature

    // Helper function to compute cross product
    auto cross = [](const std::vector<T> &a, const std::vector<T> &b) {
        return std::vector<T>{a[1] * b[2] - a[2] * b[1],
                              a[2] * b[0] - a[0] * b[2],
                              a[0] * b[1] - a[1] * b[0]};
    };

    // Helper function to compute magnitude of vector
    auto magnitude = [](const std::vector<T> &v) {
        return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    };

    // For each point (except endpoints)
    for (uint32_t i = 0; i < count; ++i) {
        // Find neighboring points for derivatives
        uint32_t prev = (i > 0) ? i - 1 : 0;
        uint32_t next = (i < count - 1) ? i + 1 : count - 1;
        uint32_t prev2 = (prev > 0) ? prev - 1 : 0;
        uint32_t next2 = (next < count - 1) ? next + 1 : count - 1;

        T t_prev = t.value(prev);
        T t_curr = t.value(i);
        T t_next = t.value(next);
        T t_prev2 = t.value(prev2);
        T t_next2 = t.value(next2);

        auto p_prev = points.array(prev);
        auto p_curr = points.array(i);
        auto p_next = points.array(next);
        auto p_prev2 = points.array(prev2);
        auto p_next2 = points.array(next2);

        // Compute first derivative using central differences
        std::vector<T> first_deriv(3);
        T dt_forward = t_next - t_curr;
        T dt_backward = t_curr - t_prev;

        if (i == 0) {
            // Forward difference at start
            for (int j = 0; j < 3; ++j) {
                first_deriv[j] = (p_next[j] - p_curr[j]) / dt_forward;
            }
        } else if (i == count - 1) {
            // Backward difference at end
            for (int j = 0; j < 3; ++j) {
                first_deriv[j] = (p_curr[j] - p_prev[j]) / dt_backward;
            }
        } else {
            // Central difference
            for (int j = 0; j < 3; ++j) {
                first_deriv[j] =
                    (p_next[j] - p_prev[j]) / (dt_forward + dt_backward);
            }
        }

        // Compute second derivative
        std::vector<T> second_deriv(3);
        if (i == 0 || i == count - 1) {
            // Use one-sided differences at endpoints
            T dt = (i == 0) ? t_next - t_curr : t_curr - t_prev;
            auto p1 = (i == 0) ? p_curr : p_prev;
            auto p2 = (i == 0) ? p_next : p_curr;
            auto p3 = (i == 0) ? p_next2 : p_prev2;
            T t1 = (i == 0) ? t_curr : t_prev;
            T t2 = (i == 0) ? t_next : t_curr;
            T t3 = (i == 0) ? t_next2 : t_prev2;

            for (int j = 0; j < 3; ++j) {
                T a = (p3[j] - p2[j]) / (t3 - t2) - (p2[j] - p1[j]) / (t2 - t1);
                second_deriv[j] = a / dt;
            }
        } else {
            // Central second derivative
            T dt = (t_next - t_prev) / 2;
            for (int j = 0; j < 3; ++j) {
                second_deriv[j] =
                    (p_next[j] - 2 * p_curr[j] + p_prev[j]) / (dt * dt);
            }
        }

        // Compute curvature: κ = |r' × r''| / |r'|³
        auto cross_prod = cross(first_deriv, second_deriv);
        T numerator = magnitude(cross_prod);
        T denominator = std::pow(magnitude(first_deriv), 3);

        T kappa = (denominator > 1e-10) ? numerator / denominator : 0;
        result.setValue(i, kappa);
    }

    return result;
}

// Helper function to create a curvature operation
template <typename T>
auto make_curve_curvature(const GenSerie<T> &parameterization = GenSerie<T>()) {
    return [&parameterization](const GenSerie<T> &points) {
        return curve_curvature(points, parameterization);
    };
}

} // namespace geo
} // namespace df
