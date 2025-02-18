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

#include <Eigen/Dense>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/types.h>
#include <vector>

namespace df {

template <typename Vec, typename T>
inline std::pair<Serie<double>, Serie<double>>
calculate_experimental_variogram(const Serie<Vec> &positions,
                                 const Serie<T> &values, double lag_distance,
                                 size_t n_lags) {
    std::vector<std::vector<double>> bins(n_lags);
    std::vector<double> distances(n_lags, 0.0);

    // Calculate squared differences for each pair of points
    for (size_t i = 0; i < positions.size(); ++i) {
        for (size_t j = i + 1; j < positions.size(); ++j) {
            double dist = 0.0;
            for (size_t k = 0; k < std::tuple_size<Vec>::value; ++k) {
                double diff = positions[i][k] - positions[j][k];
                dist += diff * diff;
            }
            dist = std::sqrt(dist);

            size_t bin = static_cast<size_t>(dist / lag_distance);
            if (bin < n_lags) {
                double val_diff = values[i] - values[j];
                bins[bin].push_back(0.5 * val_diff * val_diff);
                distances[bin] += dist;
            }
        }
    }

    // Calculate average variogram values for each bin
    std::vector<double> variogram(n_lags, 0.0);
    for (size_t i = 0; i < n_lags; ++i) {
        if (!bins[i].empty()) {
            variogram[i] =
                std::accumulate(bins[i].begin(), bins[i].end(), 0.0) /
                bins[i].size();
            distances[i] /= bins[i].size();
        }
    }

    return {Serie<double>(distances), Serie<double>(variogram)};
}

/**
 * @brief Theoretical variogram function
 */
inline double variogram_model(double distance, const VariogramParams &params) {
    if (distance == 0.0)
        return 0.0;

    double normalized_dist = distance / params.range;
    double result = params.nugget;

    switch (params.model) {
    case VariogramModel::Spherical:
        if (normalized_dist >= 1.0) {
            result += params.sill;
        } else {
            result += params.sill * (1.5 * normalized_dist -
                                     0.5 * std::pow(normalized_dist, 3));
        }
        break;

    case VariogramModel::Exponential:
        result += params.sill * (1.0 - std::exp(-3.0 * normalized_dist));
        break;

    case VariogramModel::Gaussian:
        result += params.sill *
                  (1.0 - std::exp(-3.0 * normalized_dist * normalized_dist));
        break;
    }

    return result;
}

template <typename Vec, typename T>
inline std::pair<Serie<T>, Serie<double>> ordinary_kriging(
    const Serie<Vec> &known_positions, const Serie<T> &known_values,
    const Serie<Vec> &query_positions, const VariogramParams &params) {
    const size_t n = known_positions.size();
    std::vector<T> estimates;
    std::vector<double> variances;
    estimates.reserve(query_positions.size());
    variances.reserve(query_positions.size());

    // Build kriging matrix
    Eigen::MatrixXd K(n + 1, n + 1);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            double dist = 0.0;
            for (size_t k = 0; k < std::tuple_size<Vec>::value; ++k) {
                double diff = known_positions[i][k] - known_positions[j][k];
                dist += diff * diff;
            }
            dist = std::sqrt(dist);
            K(i, j) = variogram_model(dist, params);
        }
        K(i, n) = K(n, i) = 1.0;
    }
    K(n, n) = 0.0;

    // Solve for each query point
    for (size_t q = 0; q < query_positions.size(); ++q) {
        // Build right-hand side
        Eigen::VectorXd b(n + 1);
        for (size_t i = 0; i < n; ++i) {
            double dist = 0.0;
            for (size_t k = 0; k < std::tuple_size<Vec>::value; ++k) {
                double diff = query_positions[q][k] - known_positions[i][k];
                dist += diff * diff;
            }
            dist = std::sqrt(dist);
            b(i) = variogram_model(dist, params);
        }
        b(n) = 1.0;

        // Solve kriging system
        Eigen::VectorXd weights = K.ldlt().solve(b);

        // Calculate estimate
        T estimate = 0;
        for (size_t i = 0; i < n; ++i) {
            estimate += weights(i) * known_values[i];
        }
        estimates.push_back(estimate);

        // Calculate kriging variance
        double variance = weights.dot(b);
        variances.push_back(variance);
    }

    return {Serie<T>(estimates), Serie<double>(variances)};
}

} // namespace df