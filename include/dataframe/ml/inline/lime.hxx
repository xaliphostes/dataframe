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

#include <algorithm>
#include <cmath>
#include <dataframe/core/filter.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/core/reduce.h>
#include <dataframe/stats/stats.h>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <tuple>

namespace ml {

//
// Lime Implementation
//

inline Lime::Lime(const df::Dataframe &training_data,
                  const std::string &target_column,
                  const std::set<std::string> &categorical_features,
                  double kernel_width, bool verbose)
    : training_data_(training_data), target_column_(target_column),
      categorical_features_(categorical_features), kernel_width_(kernel_width),
      verbose_(verbose), rng_(std::random_device{}()) {
    if (verbose_) {
        std::cout << "LIME explainer created with " << training_data_.size()
                  << " features and kernel width " << kernel_width_
                  << std::endl;
    }
}

inline std::vector<std::pair<std::string, double>> Lime::explain(
    const df::Dataframe &instance,
    std::function<df::Serie<double>(const df::Dataframe &)> predict_fn,
    size_t num_features, size_t num_samples) {
    if (verbose_) {
        std::cout << "Generating " << num_samples << " perturbed samples..."
                  << std::endl;
    }

    // Generate perturbed samples around the instance
    df::Dataframe perturbed_samples =
        generate_perturbed_samples(instance, num_samples);

    if (verbose_) {
        std::cout << "Calculating sample weights..." << std::endl;
    }

    // Calculate weights based on distance to the original instance
    df::Serie<double> weights =
        calculate_sample_weights(perturbed_samples, instance);

    if (verbose_) {
        std::cout << "Getting model predictions for perturbed samples..."
                  << std::endl;
    }

    // Get model predictions for the perturbed samples
    df::Serie<double> predictions = predict_fn(perturbed_samples);

    if (verbose_) {
        std::cout << "Fitting interpretable model..." << std::endl;
    }

    // Fit a simple interpretable model to explain the prediction
    auto explanation = fit_interpretable_model(perturbed_samples, predictions,
                                               weights, num_features);

    if (verbose_) {
        std::cout << "Explanation generated with " << explanation.size()
                  << " features." << std::endl;
    }

    return explanation;
}

inline df::Dataframe
Lime::generate_perturbed_samples(const df::Dataframe &instance,
                                 size_t num_samples) {
    // Create a copy of the instance repeated num_samples times
    df::Dataframe samples;

    // Get feature names (excluding the target column)
    std::vector<std::string> feature_names;
    for (const auto &name : training_data_.names()) {
        if (name != target_column_) {
            feature_names.push_back(name);
        }
    }

    // For each feature, create a perturbed series
    for (const auto &feature : feature_names) {
        // Check if the feature is categorical
        bool is_categorical =
            categorical_features_.find(feature) != categorical_features_.end();

        if (is_categorical) {
            // For categorical features, sample values from the training data
            auto all_values = training_data_.get<std::string>(feature);
            std::vector<std::string> unique_values(all_values.data().begin(),
                                                   all_values.data().end());
            std::sort(unique_values.begin(), unique_values.end());
            unique_values.erase(
                std::unique(unique_values.begin(), unique_values.end()),
                unique_values.end());

            std::vector<std::string> sampled_values;
            sampled_values.reserve(num_samples);

            std::uniform_int_distribution<size_t> index_dist(
                0, unique_values.size() - 1);

            // For some samples, keep the original value; for others, sample
            // randomly
            for (size_t i = 0; i < num_samples; ++i) {
                if (std::bernoulli_distribution(0.5)(rng_)) {
                    // Keep original value
                    sampled_values.push_back(
                        instance.get<std::string>(feature)[0]);
                } else {
                    // Sample a random value
                    sampled_values.push_back(unique_values[index_dist(rng_)]);
                }
            }

            samples.add(feature, df::Serie<std::string>(sampled_values));
        } else {
            // For numerical features, add Gaussian noise
            double original_value = instance.get<double>(feature)[0];

            // Calculate the standard deviation from the training data
            auto feature_serie = training_data_.get<double>(feature);
            double mean = df::stats::mean(feature_serie);
            double variance = df::stats::variance(feature_serie);
            double stddev = std::sqrt(variance);

            // Sample from a normal distribution centered at the original value
            std::normal_distribution<double> dist(original_value, stddev);

            std::vector<double> sampled_values;
            sampled_values.reserve(num_samples);

            for (size_t i = 0; i < num_samples; ++i) {
                sampled_values.push_back(dist(rng_));
            }

            samples.add(feature, df::Serie<double>(sampled_values));
        }
    }

    return samples;
}

inline df::Serie<double>
Lime::calculate_sample_weights(const df::Dataframe &perturbed_samples,
                               const df::Dataframe &original_instance) {
    std::vector<double> weights;
    weights.reserve(perturbed_samples.get(perturbed_samples.names()[0]).size());

    // Each row in perturbed_samples represents a sample
    size_t num_samples =
        perturbed_samples.get(perturbed_samples.names()[0]).size();

    for (size_t i = 0; i < num_samples; ++i) {
        // Create a dataframe with a single row for the current sample
        df::Dataframe sample_df;

        for (const auto &name : perturbed_samples.names()) {
            if (perturbed_samples.type_name(name) == "Serie<double>") {
                auto value = perturbed_samples.get<double>(name)[i];
                sample_df.add(name, df::Serie<double>{value});
            } else if (perturbed_samples.type_name(name) ==
                       "Serie<std::string>") {
                auto value = perturbed_samples.get<std::string>(name)[i];
                sample_df.add(name, df::Serie<std::string>{value});
            }
        }

        double distance = calculate_distance(sample_df, original_instance);
        double weight =
            std::exp(-(distance * distance) / (kernel_width_ * kernel_width_));
        weights.push_back(weight);
    }

    return df::Serie<double>(weights);
}

inline double Lime::calculate_distance(const df::Dataframe &instance1,
                                       const df::Dataframe &instance2) {
    double squared_distance = 0.0;

    for (const auto &feature : instance1.names()) {
        if (instance1.type_name(feature) == "Serie<double>") {
            // For numerical features, use squared difference normalized by
            // variance
            double val1 = instance1.get<double>(feature)[0];
            double val2 = instance2.get<double>(feature)[0];

            // Get the variance of this feature from the training data
            auto feature_serie = training_data_.get<double>(feature);
            double mean = df::stats::mean(feature_serie);
            double variance = df::stats::variance(feature_serie);

            // Normalize by variance to make features comparable
            if (variance > 0) {
                squared_distance += ((val1 - val2) * (val1 - val2)) / variance;
            } else {
                squared_distance += (val1 - val2) * (val1 - val2);
            }
        } else if (instance1.type_name(feature) == "Serie<std::string>") {
            // For categorical features, use a simple 0/1 distance
            std::string val1 = instance1.get<std::string>(feature)[0];
            std::string val2 = instance2.get<std::string>(feature)[0];

            if (val1 != val2) {
                squared_distance += 1.0;
            }
        }
    }

    return std::sqrt(squared_distance);
}

inline std::vector<std::pair<std::string, double>>
Lime::fit_interpretable_model(const df::Dataframe &perturbed_samples,
                              const df::Serie<double> &predictions,
                              const df::Serie<double> &weights,
                              size_t num_features) {
    // Convert categorical features to binary features
    std::vector<std::string> feature_names;
    std::vector<std::vector<double>> X;

    // Prepare the feature matrix X for the linear model
    for (const auto &feature : perturbed_samples.names()) {
        if (categorical_features_.find(feature) !=
            categorical_features_.end()) {
            // For categorical features, create binary features for each unique
            // value
            auto values = perturbed_samples.get<std::string>(feature);

            // Get unique values
            std::set<std::string> unique_values;
            for (size_t i = 0; i < values.size(); ++i) {
                unique_values.insert(values[i]);
            }

            // Create binary features for each value except one (to avoid
            // collinearity)
            std::vector<std::string> binary_feature_names;
            std::vector<std::vector<double>> binary_features;

            auto unique_values_vec = std::vector<std::string>(
                unique_values.begin(), unique_values.end());
            if (!unique_values_vec.empty()) {
                // Skip the last value to avoid perfect collinearity
                for (size_t j = 0; j < unique_values_vec.size() - 1; ++j) {
                    std::string value = unique_values_vec[j];
                    std::string binary_feature_name = feature + "=" + value;
                    binary_feature_names.push_back(binary_feature_name);

                    std::vector<double> binary_feature(values.size(), 0.0);
                    for (size_t i = 0; i < values.size(); ++i) {
                        if (values[i] == value) {
                            binary_feature[i] = 1.0;
                        }
                    }

                    binary_features.push_back(binary_feature);
                }

                // Add the binary features to X
                for (size_t j = 0; j < binary_feature_names.size(); ++j) {
                    feature_names.push_back(binary_feature_names[j]);
                    X.push_back(binary_features[j]);
                }
            }
        } else {
            // For numerical features, use them directly
            feature_names.push_back(feature);

            auto values = perturbed_samples.get<double>(feature);
            std::vector<double> feature_values(values.data().begin(),
                                               values.data().end());
            X.push_back(feature_values);
        }
    }

    // Transpose X to have samples as rows
    std::vector<std::vector<double>> X_transposed;
    if (!X.empty()) {
        X_transposed.resize(X[0].size(), std::vector<double>(X.size()));
        for (size_t i = 0; i < X.size(); ++i) {
            for (size_t j = 0; j < X[i].size(); ++j) {
                X_transposed[j][i] = X[i][j];
            }
        }
    }

    // Convert predictions and weights to std::vector
    std::vector<double> y(predictions.data().begin(), predictions.data().end());
    std::vector<double> w(weights.data().begin(), weights.data().end());

    // Fit a weighted linear regression model
    SimpleLinearRegression model;
    model.fit(X_transposed, y, w);

    // Get the coefficients
    std::vector<double> coefficients = model.get_coefficients();

    // Create pairs of feature names and coefficients
    std::vector<std::pair<std::string, double>> feature_importances;
    for (size_t i = 0; i < feature_names.size(); ++i) {
        feature_importances.emplace_back(feature_names[i],
                                         std::abs(coefficients[i]));
    }

    // Sort by absolute coefficient value (importance)
    std::sort(feature_importances.begin(), feature_importances.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });

    // Limit to the top num_features
    if (feature_importances.size() > num_features) {
        feature_importances.resize(num_features);
    }

    // Now sort by the actual coefficient values to see positive and negative
    // influences
    for (auto &pair : feature_importances) {
        // Find the original coefficient
        auto it =
            std::find(feature_names.begin(), feature_names.end(), pair.first);
        if (it != feature_names.end()) {
            size_t idx = std::distance(feature_names.begin(), it);
            pair.second = coefficients[idx];
        }
    }

    return feature_importances;
}

//
// SimpleLinearRegression Implementation
//
inline void
SimpleLinearRegression::fit(const std::vector<std::vector<double>> &X,
                            const std::vector<double> &y,
                            const std::vector<double> &weights) {
    // Simple weighted least squares implementation
    if (X.empty() || y.empty() || weights.empty() || X.size() != y.size() ||
        y.size() != weights.size()) {
        throw std::invalid_argument("Input data dimensions don't match");
    }

    size_t n_samples = X.size();
    size_t n_features = X[0].size();

    // Initialize the coefficient vector with zeros
    coefficients_ = std::vector<double>(n_features, 0.0);

    // Calculate weighted mean of X and y
    std::vector<double> X_weighted_mean(n_features, 0.0);
    double y_weighted_mean = 0.0;
    double sum_weights = 0.0;

    for (size_t i = 0; i < n_samples; ++i) {
        for (size_t j = 0; j < n_features; ++j) {
            X_weighted_mean[j] += weights[i] * X[i][j];
        }
        y_weighted_mean += weights[i] * y[i];
        sum_weights += weights[i];
    }

    for (size_t j = 0; j < n_features; ++j) {
        X_weighted_mean[j] /= sum_weights;
    }
    y_weighted_mean /= sum_weights;

    // Simple approach for a small number of features: normal equations
    if (n_features < 100) {
        // Calculate covariance matrix and cross-covariance
        std::vector<std::vector<double>> cov_matrix(
            n_features, std::vector<double>(n_features, 0.0));
        std::vector<double> cross_cov(n_features, 0.0);

        for (size_t i = 0; i < n_samples; ++i) {
            for (size_t j = 0; j < n_features; ++j) {
                cross_cov[j] += weights[i] * (X[i][j] - X_weighted_mean[j]) *
                                (y[i] - y_weighted_mean);

                for (size_t k = 0; k < n_features; ++k) {
                    cov_matrix[j][k] += weights[i] *
                                        (X[i][j] - X_weighted_mean[j]) *
                                        (X[i][k] - X_weighted_mean[k]);
                }
            }
        }

        // Solve linear system (simple matrix inversion for small matrices)
        // In practice, you would use a library like Eigen for this
        // This is a simplified approach that may not work well for
        // ill-conditioned matrices

        // For simplicity, we'll just use a simple implementation for the
        // 1-feature case
        if (n_features == 1) {
            if (std::abs(cov_matrix[0][0]) > 1e-10) {
                coefficients_[0] = cross_cov[0] / cov_matrix[0][0];
            }
        } else {
            // For multi-feature case, we'll use a simple ridge regression
            // Add a small regularization term to the diagonal
            double alpha = 0.001;
            for (size_t j = 0; j < n_features; ++j) {
                cov_matrix[j][j] += alpha;
            }

            // Simple Gaussian elimination with partial pivoting
            // Copy the matrix and augment it with the right-hand side
            std::vector<std::vector<double>> augmented(
                n_features, std::vector<double>(n_features + 1));
            for (size_t i = 0; i < n_features; ++i) {
                for (size_t j = 0; j < n_features; ++j) {
                    augmented[i][j] = cov_matrix[i][j];
                }
                augmented[i][n_features] = cross_cov[i];
            }

            // Forward elimination
            for (size_t i = 0; i < n_features; ++i) {
                // Find pivot
                size_t max_row = i;
                double max_val = std::abs(augmented[i][i]);

                for (size_t k = i + 1; k < n_features; ++k) {
                    if (std::abs(augmented[k][i]) > max_val) {
                        max_val = std::abs(augmented[k][i]);
                        max_row = k;
                    }
                }

                // Swap rows
                if (max_row != i) {
                    std::swap(augmented[i], augmented[max_row]);
                }

                // Eliminate below
                for (size_t k = i + 1; k < n_features; ++k) {
                    double factor = augmented[k][i] / augmented[i][i];
                    augmented[k][i] = 0.0;

                    for (size_t j = i + 1; j <= n_features; ++j) {
                        augmented[k][j] -= factor * augmented[i][j];
                    }
                }
            }

            // Back substitution
            for (int i = n_features - 1; i >= 0; --i) {
                double sum = augmented[i][n_features];

                for (size_t j = i + 1; j < n_features; ++j) {
                    sum -= augmented[i][j] * coefficients_[j];
                }

                coefficients_[i] = sum / augmented[i][i];
            }
        }
    } else {
        // For large number of features, use a more sophisticated approach
        // like gradient descent or a library like Eigen
        // Omitted for simplicity and for now
    }
}

inline std::vector<double> SimpleLinearRegression::get_coefficients() const {
    return coefficients_;
}

inline std::vector<double> SimpleLinearRegression::predict(
    const std::vector<std::vector<double>> &X) const {
    std::vector<double> predictions;
    predictions.reserve(X.size());

    for (const auto &x : X) {
        double pred = 0.0;
        for (size_t j = 0; j < coefficients_.size(); ++j) {
            pred += coefficients_[j] * x[j];
        }
        predictions.push_back(pred);
    }

    return predictions;
}

inline double
SimpleLinearRegression::score(const std::vector<std::vector<double>> &X,
                              const std::vector<double> &y) const {
    if (X.empty() || y.empty() || X.size() != y.size()) {
        throw std::invalid_argument("Input data dimensions don't match");
    }

    auto predictions = predict(X);

    double ss_total = 0.0;
    double ss_residual = 0.0;

    // Calculate mean of y
    double y_mean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();

    for (size_t i = 0; i < y.size(); ++i) {
        ss_total += (y[i] - y_mean) * (y[i] - y_mean);
        ss_residual += (y[i] - predictions[i]) * (y[i] - predictions[i]);
    }

    if (ss_total < 1e-10) {
        return 1.0 - (ss_residual / ss_total);
    }

    return 1;
}

} // namespace ml
