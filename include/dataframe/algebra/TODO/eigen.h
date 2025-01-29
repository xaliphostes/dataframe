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
#include <dataframe/map.h>

namespace df {
namespace algebra {

template <typename T>
using EigenSystem = std::tuple<Serie<T>, Serie<T>>;

/**
 * @brief Compute the eigen values of a symmetric matrix
 * @param serie
 * @return Serie<double> The eigen values (v1, v2, v3) for each items
 * @code
 * // Create a 3x3 symmetric matrix serie
 * df::Serie<double> serie(6, {
 *     2, 4, 6, 3, 6, 9,
 *     1, 2, 3, 4, 5, 6,
 *     9, 8, 7, 6, 5, 4
 * });
 * 
 * auto values = df::algebra::eigenValues(serie);
 * df::print(values);
 * @endcode
 * 
 * will displayed
 * 
 * ```
 * Serie<double> {
 *   itemSize : 9
 *   count    : 3
 *   dimension: 3
 *   values   : [
 *     [16.3328, -0.6580, -1.6748],
 *     [11.3448, 0.1709, -0.5157],
 *     [20.1911, -0.0431, -1.1480]
 *   ]
 * }
 * ```
 */
template <typename T>
Serie<T> eigenValues(const Serie<T> &serie);

/**
 * @brief Compute the eigen vectors of a symmetric matrix
 * @param serie
 * @return Serie<double> The eigen vectors (v1x,v1y,v1z, v2x,v2y,v2z,
 * v3x,v3y,v3z) for each items
 * @code
 * // Create a 3x3 symmetric matrix serie
 * df::Serie<double> serie(6, {
 *     2, 4, 6, 3, 6, 9,
 *     1, 2, 3, 4, 5, 6,
 *     9, 8, 7, 6, 5, 4
 * });
 * 
 * auto vectors = df::algebra::eigenVectors(serie);
 * df::print(vectors);
 * @endcode
 * 
 * will displayed
 * 
 * ```
 * Serie<double> {
 *   itemSize : 9
 *   count    : 3
 *   dimension: 3
 *   values   : [
 *     [0.4493, 0.4752, 0.7565, 0.1945, 0.7745, -0.6020, 0.8720, -0.4176, -0.2556],
 *     [0.3280, 0.5910, 0.7370, -0.5921, 0.7365, -0.3271, 0.7361, 0.3291, -0.5915],
 *     [0.6888, 0.5534, 0.4683, 0.1594, -0.7457, 0.6469, -0.7072, 0.3709, 0.6019]
 *   ]
 * }
 * ```
 */
template <typename T>
Serie<T> eigenVectors(const Serie<T> &serie);

/**
 * @brief Compute the eigen values and vectors of a symmetric matrix
 * @param serie
 * @return EigenSystem The eigen values and eigen vectors
 * @code
 * auto [values, vectors] = df::algebra::eigenSystem(serie);
 * @endcode
 */
template <typename T>
EigenSystem<T> eigenSystem(const Serie<T> &serie);

/**
 * @brief Make the eigen values function in order to be used with pipe
 */
template <typename T>
auto make_eigenValues();

/**
 * @brief Make the eigen vectors function in order to be used with pipe
 */
template <typename T>
auto make_eigenVectors();

/**
 * @brief Make the eigen system function in order to be used with pipe
 */
template <typename T>
auto make_eigenSystem();

} // namespace algebra
} // namespace df

#include "inline/eigen.hxx"