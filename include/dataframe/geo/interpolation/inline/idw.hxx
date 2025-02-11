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
#include <cmath>
#include <vector>

namespace df {

template <typename T>
inline Serie<T> idw_2d(const Serie<Vector2> &points, const Serie<T> &values,
                       const Serie<Vector2> &targets, double power,
                       double smoothing) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    Serie<T> result(targets.size());

    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        double weight_sum = 0.0;
        T value_sum = 0;

        // Check if target point coincides with any input point
        bool exact_match = false;
        for (size_t j = 0; j < points.size(); ++j) {
            if (distance_squared_2d(target, points[j]) < smoothing) {
                result[i] = values[j];
                exact_match = true;
                break;
            }
        }

        if (!exact_match) {
            // Compute weighted sum
            for (size_t j = 0; j < points.size(); ++j) {
                double dist = std::sqrt(distance_squared_2d(target, points[j]) +
                                        smoothing);
                double weight = 1.0 / std::pow(dist, power);
                weight_sum += weight;
                value_sum += weight * values[j];
            }
            result[i] = value_sum / weight_sum;
        }
    }

    return result;
}

template <typename T>
inline Serie<T> idw_3d(const Serie<Vector3> &points, const Serie<T> &values,
                       const Serie<Vector3> &targets, double power,
                       double smoothing) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    Serie<T> result(targets.size());

    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        double weight_sum = 0.0;
        T value_sum = 0;

        // Check if target point coincides with any input point
        bool exact_match = false;
        for (size_t j = 0; j < points.size(); ++j) {
            if (distance_squared_3d(target, points[j]) < smoothing) {
                result[i] = values[j];
                exact_match = true;
                break;
            }
        }

        if (!exact_match) {
            // Compute weighted sum
            for (size_t j = 0; j < points.size(); ++j) {
                double dist = std::sqrt(distance_squared_3d(target, points[j]) +
                                        smoothing);
                double weight = 1.0 / std::pow(dist, power);
                weight_sum += weight;
                value_sum += weight * values[j];
            }
            result[i] = value_sum / weight_sum;
        }
    }

    return result;
}

namespace detail {

    template <typename T, size_t DIM> struct idw_traits {};
    
    template <typename T> struct idw_traits<T, 2> {
        using type = Vector2;
        Serie<T> idw(const Serie<type> &points, const Serie<T> &values,
                     const Serie<type> &targets, double power = 2.0,
                     double smoothing = 1e-10) {
            return idw_2d(points, values, targets, power, smoothing);
        }
    };
    
    template <typename T> struct idw_traits<T, 3> {
        using type = Vector3;
        Serie<T> idw(const Serie<type> &points, const Serie<T> &values,
                     const Serie<type> &targets, double power = 2.0,
                     double smoothing = 1e-10) {
            return idw_3d(points, values, targets, power, smoothing);
        }
    };
    
    } // namespace detail

} // namespace df