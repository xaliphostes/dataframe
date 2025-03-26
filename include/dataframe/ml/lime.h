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
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <set>
#include <random>

namespace ml {

/**
 * @brief LIME (Local Interpretable Model-agnostic Explanations) implementation
 * for explaining individual predictions of any black-box model.
 * 
 * A C++ implementation of LIME (Local Interpretable Model-agnostic
 * Explanations) integrated with the DataFrame library
 *
 * LIME works by approximating a complex model locally with a simpler,
 * interpretable model (like linear regression) around a specific prediction.
 * This implementation is designed to work with the DataFrame library and
 * supports both numerical and categorical features.
 *
 * Example usage:
 * @code
 * #include <dataframe/Serie.h>
 * #include <dataframe/Dataframe.h>
 * #include <dataframe/ml/lime.h>
 * #include <dataframe/ml/random_forest.h>
 *
 * int main() {
 *     // Load data and train a model
 *     df::Dataframe data = df::io::read_csv("data.csv");
 *     auto model = ml::create_random_forest_regressor(100);
 *     model.fit(data, "target");
 *
 *     // Create a LIME explainer
 *     ml::Lime lime_explainer(data, "target");
 *
 *     // Select an instance to explain
 *     df::Dataframe instance = data.get_row(10);
 *
 *     // Get the explanation for the instance
 *     auto explanation = lime_explainer.explain(
 *         instance,
 *         [&model](const df::Dataframe& samples) {
 *             return model.predict(samples);
 *         },
 *         5  // num_features to include in explanation
 *     );
 *
 *     // Print the explanation
 *     for (const auto& [feature, weight] : explanation) {
 *         std::cout << feature << ": " << weight << std::endl;
 *     }
 *
 *     return 0;
 * }
 * @endcode
 */
class Lime {
  public:
    /**
     * @brief Construct a Lime explainer
     *
     * @param training_data The training data used for generating explanations
     * @param target_column The name of the target column in the training data
     * @param categorical_features Optional set of feature names that should be
     * treated as categorical
     * @param kernel_width Width of the exponential kernel (default: 0.75)
     * @param verbose Whether to print progress information (default: false)
     */
    Lime(const df::Dataframe &training_data, const std::string &target_column,
         const std::set<std::string> &categorical_features = {},
         double kernel_width = 0.75, bool verbose = false);

    /**
     * @brief Generate an explanation for a specific instance
     *
     * @param instance The instance to explain
     * @param predict_fn A function that takes a Dataframe of samples and
     * returns predictions
     * @param num_features Number of features to include in the explanation
     * (default: 5)
     * @param num_samples Number of samples to generate for local approximation
     * (default: 5000)
     * @return std::vector<std::pair<std::string, double>> Feature names and
     * their weights in the explanation
     */
    std::vector<std::pair<std::string, double>>
    explain(const df::Dataframe &instance,
            std::function<df::Serie<double>(const df::Dataframe &)> predict_fn,
            size_t num_features = 5, size_t num_samples = 5000);

  private:
    df::Dataframe training_data_;
    std::string target_column_;
    std::set<std::string> categorical_features_;
    double kernel_width_;
    bool verbose_;
    std::mt19937 rng_;

    /**
     * @brief Generate perturbed samples around an instance
     *
     * @param instance The instance to explain
     * @param num_samples Number of samples to generate
     * @return df::Dataframe A dataframe containing the perturbed samples
     */
    df::Dataframe generate_perturbed_samples(const df::Dataframe &instance,
                                             size_t num_samples);

    /**
     * @brief Calculate weights for perturbed samples based on their distance to
     * the original instance
     *
     * @param perturbed_samples The perturbed samples
     * @param original_instance The original instance being explained
     * @return df::Serie<double> Weights for each perturbed sample
     */
    df::Serie<double>
    calculate_sample_weights(const df::Dataframe &perturbed_samples,
                             const df::Dataframe &original_instance);

    /**
     * @brief Fit a simple interpretable model (linear regression) to explain
     * the prediction
     *
     * @param perturbed_samples Perturbed samples around the instance
     * @param predictions Model predictions for the perturbed samples
     * @param weights Sample weights based on distance to the original instance
     * @param num_features Number of features to include in the explanation
     * @return std::vector<std::pair<std::string, double>> Feature names and
     * their weights
     */
    std::vector<std::pair<std::string, double>>
    fit_interpretable_model(const df::Dataframe &perturbed_samples,
                            const df::Serie<double> &predictions,
                            const df::Serie<double> &weights,
                            size_t num_features);

    /**
     * @brief Calculate the Euclidean distance between two instances
     *
     * @param instance1 First instance
     * @param instance2 Second instance
     * @return double The Euclidean distance
     */
    double calculate_distance(const df::Dataframe &instance1,
                              const df::Dataframe &instance2);

    /**
     * @brief Get feature values from a dataframe row as a vector
     *
     * @param instance The dataframe row
     * @return std::vector<double> Feature values
     */
    std::vector<double> get_feature_values(const df::Dataframe &instance);

    /**
     * @brief Convert a categorical feature to binary features
     *
     * @param feature_name Name of the categorical feature
     * @param feature_value Value of the categorical feature
     * @param unique_values Set of unique values for this feature
     * @return std::vector<std::pair<std::string, double>> Binary features and
     * their values
     */
    std::vector<std::pair<std::string, double>>
    binarize_categorical_feature(const std::string &feature_name,
                                 const std::string &feature_value,
                                 const std::set<std::string> &unique_values);
};

// Function to create a LIME explainer for regression
inline Lime
create_lime_explainer(const df::Dataframe &training_data,
                      const std::string &target_column,
                      const std::set<std::string> &categorical_features = {}) {

    return Lime(training_data, target_column, categorical_features);
}

// Utility class for simple weighted linear regression
class SimpleLinearRegression {
  public:
    SimpleLinearRegression() = default;

    void fit(const std::vector<std::vector<double>> &X,
             const std::vector<double> &y, const std::vector<double> &weights);

    std::vector<double> get_coefficients() const;

    std::vector<double>
    predict(const std::vector<std::vector<double>> &X) const;

    double score(const std::vector<std::vector<double>> &X,
                 const std::vector<double> &y) const;

  private:
    std::vector<double> coefficients_;
};

} // namespace ml

#include "inline/lime.hxx"