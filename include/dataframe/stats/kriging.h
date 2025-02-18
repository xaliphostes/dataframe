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
 */

#pragma once
#include <dataframe/Serie.h>

/**
 * @brief kriging implementation:
 *
 * # VariogramModel and VariogramParams: These define the theoretical models for
 * spatial correlation and their parameters:
 * - Nugget: Represents measurement error or microscale variation
 * - Sill: The maximum variogram value (plateau)
 * - Range: The distance at which spatial correlation levels off
 * - Three common variogram models: Spherical, Exponential, and Gaussian
 *
 * # calculate_experimental_variogram: Calculates the empirical variogram from
 * data:
 * - Takes positions and values as Serie inputs
 * - Bins point pairs by distance
 * - Returns distances and corresponding variogram values
 *
 * # variogram_model: Implements theoretical variogram models:
 * - Spherical: Linear near origin, reaches sill at range
 * - Exponential: Approaches sill asymptotically
 * - Gaussian: Parabolic near origin, smooth approach to sill
 *
 * # ordinary_kriging: The main interpolation function:
 * - Builds kriging matrix using variogram values
 * - Solves kriging system for each query point
 * - Returns both estimates and kriging variances
 */
namespace df {

/**
 * @brief Variogram models for kriging
 */
enum class VariogramModel { Spherical, Exponential, Gaussian };

/**
 * @brief Parameters for variogram modeling
 */
struct VariogramParams {
    double nugget = 0.0; // Nugget effect (y-intercept)
    double sill = 1.0;   // Maximum variogram value minus nugget
    double range = 1.0;  // Distance at which variogram levels off
    VariogramModel model = VariogramModel::Spherical;
};

/**
 * @brief Theoretical variogram function
 */
double variogram_model(double distance, const VariogramParams &params);

/**
 * @brief Calculate experimental variogram from data points
 * @param positions Serie of point positions (Vector2 or Vector3)
 * @param values Serie of corresponding values
 * @param lag_distance Distance interval for binning
 * @param n_lags Number of lag intervals
 * @return Pair of Series: distances and variogram values
 */
template <typename Vec, typename T>
std::pair<Serie<double>, Serie<double>>
calculate_experimental_variogram(const Serie<Vec> &positions,
                                 const Serie<T> &values, double lag_distance,
                                 size_t n_lags);

/**
 * @brief Perform ordinary kriging interpolation
 * @param known_positions Serie of known point positions
 * @param known_values Serie of known values
 * @param query_positions Serie of positions to interpolate
 * @param params Variogram parameters
 * @return Serie of interpolated values and estimation variances
 *
 * @code
 * // Example usage
 * Serie<Vector2> known_positions = {{0,0}, {1,1}, {2,0}, {0,2}};
 * Serie<double> known_values = {1.0, 2.0, 1.5, 2.5};
 * Serie<Vector2> query_positions = {{0.5, 0.5}, {1.5, 1.5}};
 *
 * VariogramParams params;
 * params.nugget = 0.1;
 * params.sill = 1.0;
 * params.range = 2.0;
 * params.model = VariogramModel::Spherical;
 *
 * auto [estimates, variances] = ordinary_kriging(
 *     known_positions,
 *     known_values,
 *     query_positions,
 *     params
 * );
 * @endcode
 */
template <typename Vec, typename T>
std::pair<Serie<T>, Serie<double>> ordinary_kriging(
    const Serie<Vec> &known_positions, const Serie<T> &known_values,
    const Serie<Vec> &query_positions, const VariogramParams &params);

} // namespace df

#include "inline/kriging.hxx"