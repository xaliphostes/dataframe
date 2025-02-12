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
#include <dataframe/Serie.h>
#include <vector>

namespace df {

/**
 * Nearest neighbor interpolation for 2D points
 */
template <typename T>
Serie<T> nearest_2d(const Serie<Vector2> &points, const Serie<T> &values,
                    const Serie<Vector2> &targets) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    Serie<T> result(targets.size());

    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        double min_dist = std::numeric_limits<double>::max();
        size_t nearest_idx = 0;

        for (size_t j = 0; j < points.size(); ++j) {
            double dist = distance_squared_2d(target, points[j]);
            if (dist < min_dist) {
                min_dist = dist;
                nearest_idx = j;
            }
        }

        result[i] = values[nearest_idx];
    }

    return result;
}

/**
 * Nearest neighbor interpolation for 3D points
 */
template <typename T>
Serie<T> nearest_3d(const Serie<Vector3> &points, const Serie<T> &values,
                    const Serie<Vector3> &targets) {
    if (points.size() != values.size()) {
        throw std::runtime_error(
            "Points and values series must have same size");
    }

    Serie<T> result(targets.size());

    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        double min_dist = std::numeric_limits<double>::max();
        size_t nearest_idx = 0;

        for (size_t j = 0; j < points.size(); ++j) {
            double dist = distance_squared_3d(target, points[j]);
            if (dist < min_dist) {
                min_dist = dist;
                nearest_idx = j;
            }
        }

        result[i] = values[nearest_idx];
    }

    return result;
}

} // namespace df