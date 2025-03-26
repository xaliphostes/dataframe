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
#include <dataframe/core/reduce.h>
#include <dataframe/core/split.h>
#include <dataframe/core/zip.h>
#include <dataframe/math/random.h>
#include <functional>
#include <limits>
#include <memory>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <random>
#include <unordered_set>
#include <vector>

namespace ml {

/**
 * @brief Node in the decision tree
 */
class DecisionNode {
  private:
    size_t feature_index; // Index of the feature to split on
    double threshold;     // Threshold for the split
    double value;         // Prediction value for leaf nodes
    std::vector<double>
        class_counts; // Class distribution for classification leaf nodes
    std::shared_ptr<DecisionNode>
        left; // Left child (feature value <= threshold)
    std::shared_ptr<DecisionNode>
        right; // Right child (feature value > threshold)

  public:
    // Constructor for internal node
    DecisionNode(size_t feature_index, double threshold)
        : feature_index(feature_index), threshold(threshold), value(0.0) {}

    // Constructor for leaf node (regression)
    DecisionNode(double value)
        : feature_index(0), threshold(0.0), value(value) {}

    // Constructor for leaf node (classification)
    DecisionNode(const std::vector<double> &class_counts)
        : feature_index(0), threshold(0.0), value(0.0),
          class_counts(class_counts) {
        // Determine the majority class as the prediction value
        if (!class_counts.empty()) {
            auto max_it =
                std::max_element(class_counts.begin(), class_counts.end());
            value = std::distance(class_counts.begin(), max_it);
        }
    }

    // Check if this is a leaf node
    bool is_leaf() const { return left == nullptr && right == nullptr; }

    // Setters for children
    void set_left(std::shared_ptr<DecisionNode> node) { left = node; }
    void set_right(std::shared_ptr<DecisionNode> node) { right = node; }

    // Getters
    size_t get_feature_index() const { return feature_index; }
    double get_threshold() const { return threshold; }
    double get_value() const { return value; }
    const std::vector<double> &get_class_counts() const { return class_counts; }
    std::shared_ptr<DecisionNode> get_left() const { return left; }
    std::shared_ptr<DecisionNode> get_right() const { return right; }

    // Predict a single sample
    double predict(const std::vector<double> &features) const {
        if (is_leaf()) {
            return value;
        }

        if (feature_index < features.size()) {
            if (features[feature_index] <= threshold) {
                return left ? left->predict(features) : value;
            } else {
                return right ? right->predict(features) : value;
            }
        }

        return value; // Default if something went wrong
    }
};

/**
 * @brief Decision Tree implementation
 */
class DecisionTree {
  private:
    std::shared_ptr<DecisionNode> root;
    TaskType task_type;
    size_t max_depth;
    size_t min_samples_split;
    size_t max_features;
    size_t n_classes; // For classification

    // Calculate the impurity for regression (MSE)
    double calculate_mse(const std::vector<double> &y) const {
        if (y.empty())
            return 0.0;

        double sum = std::accumulate(y.begin(), y.end(), 0.0);
        double mean = sum / y.size();

        double mse = 0.0;
        for (double value : y) {
            double diff = value - mean;
            mse += diff * diff;
        }

        return y.size() > 1 ? (mse / y.size()) : 0.0;
    }

    // Calculate the impurity for classification (Gini)
    double calculate_gini(const std::vector<double> &y) const {
        if (y.empty())
            return 0.0;

        std::vector<double> class_counts(n_classes, 0.0);
        for (double value : y) {
            if (value >= 0 && value < n_classes) {
                class_counts[static_cast<size_t>(value)]++;
            }
        }

        double gini = 1.0;
        double n = static_cast<double>(y.size());
        for (double count : class_counts) {
            double p = count / n;
            gini -= p * p;
        }

        return gini;
    }

    // Find the best split for a node
    std::tuple<size_t, double, double>
    find_best_split(const std::vector<std::vector<double>> &X,
                    const std::vector<double> &y,
                    const std::vector<size_t> &feature_indices) const {

        size_t best_feature = 0;
        double best_threshold = 0.0;
        double best_gain = -1.0;

        // Calculate current impurity
        double current_impurity;
        if (task_type == TaskType::REGRESSION) {
            current_impurity = calculate_mse(y);
        } else {
            current_impurity = calculate_gini(y);
        }

        size_t n_samples = y.size();

        // Try each feature
        for (size_t feature_idx : feature_indices) {
            // Get unique values for the feature
            std::vector<double> unique_values;
            for (const auto &sample : X) {
                if (feature_idx < sample.size()) {
                    unique_values.push_back(sample[feature_idx]);
                }
            }

            // Sort and get unique values
            std::sort(unique_values.begin(), unique_values.end());
            auto last = std::unique(unique_values.begin(), unique_values.end());
            unique_values.erase(last, unique_values.end());

            // Try each threshold (midpoint between consecutive values)
            for (size_t i = 0; i < unique_values.size() - 1; ++i) {
                double threshold =
                    (unique_values[i] + unique_values[i + 1]) / 2.0;

                // Split the data
                std::vector<double> left_y, right_y;
                for (size_t j = 0; j < n_samples; ++j) {
                    if (feature_idx < X[j].size() &&
                        X[j][feature_idx] <= threshold) {
                        left_y.push_back(y[j]);
                    } else {
                        right_y.push_back(y[j]);
                    }
                }

                // Skip if the split doesn't create valid children
                if (left_y.empty() || right_y.empty())
                    continue;

                // Calculate impurity for children
                double left_impurity, right_impurity;
                if (task_type == TaskType::REGRESSION) {
                    left_impurity = calculate_mse(left_y);
                    right_impurity = calculate_mse(right_y);
                } else {
                    left_impurity = calculate_gini(left_y);
                    right_impurity = calculate_gini(right_y);
                }

                // Calculate the weighted impurity
                double left_weight =
                    static_cast<double>(left_y.size()) / n_samples;
                double right_weight =
                    static_cast<double>(right_y.size()) / n_samples;
                double weighted_impurity =
                    left_weight * left_impurity + right_weight * right_impurity;

                // Calculate information gain
                double gain = current_impurity - weighted_impurity;

                // Update best split if this is better
                if (gain > best_gain) {
                    best_gain = gain;
                    best_feature = feature_idx;
                    best_threshold = threshold;
                }
            }
        }

        return {best_feature, best_threshold, best_gain};
    }

    // Build a tree recursively
    std::shared_ptr<DecisionNode>
    build_tree(const std::vector<std::vector<double>> &X,
               const std::vector<double> &y, size_t depth) {

        size_t n_samples = y.size();

        // Check stopping criteria
        if (depth >= max_depth || n_samples < min_samples_split) {
            if (task_type == TaskType::REGRESSION) {
                // Leaf node for regression: average of target values
                double mean = 0.0;
                if (!y.empty()) {
                    mean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
                }
                return std::make_shared<DecisionNode>(mean);
            } else {
                // Leaf node for classification: class distribution
                std::vector<double> class_counts(n_classes, 0.0);
                for (double value : y) {
                    if (value >= 0 && value < n_classes) {
                        class_counts[static_cast<size_t>(value)]++;
                    }
                }
                return std::make_shared<DecisionNode>(class_counts);
            }
        }

        // Select random subset of features to consider
        std::vector<size_t> feature_indices;
        if (X.empty()) {
            return std::make_shared<DecisionNode>(0.0); // Empty data
        }

        size_t n_features = X[0].size();
        size_t n_features_to_consider = std::min(max_features, n_features);

        // Initialize indices
        std::vector<size_t> all_indices(n_features);
        std::iota(all_indices.begin(), all_indices.end(), 0);

        // Randomly shuffle and take the first n_features_to_consider
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(all_indices.begin(), all_indices.end(), g);
        feature_indices.assign(all_indices.begin(),
                               all_indices.begin() + n_features_to_consider);

        // Find the best split
        auto [best_feature, best_threshold, best_gain] =
            find_best_split(X, y, feature_indices);

        // If no meaningful split was found, create a leaf
        if (best_gain <= 0.0) {
            if (task_type == TaskType::REGRESSION) {
                double mean = 0.0;
                if (!y.empty()) {
                    mean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
                }
                return std::make_shared<DecisionNode>(mean);
            } else {
                std::vector<double> class_counts(n_classes, 0.0);
                for (double value : y) {
                    if (value >= 0 && value < n_classes) {
                        class_counts[static_cast<size_t>(value)]++;
                    }
                }
                return std::make_shared<DecisionNode>(class_counts);
            }
        }

        // Create an internal node with the best split
        auto node =
            std::make_shared<DecisionNode>(best_feature, best_threshold);

        // Split the data
        std::vector<std::vector<double>> left_X, right_X;
        std::vector<double> left_y, right_y;

        for (size_t i = 0; i < n_samples; ++i) {
            if (best_feature < X[i].size() &&
                X[i][best_feature] <= best_threshold) {
                left_X.push_back(X[i]);
                left_y.push_back(y[i]);
            } else {
                right_X.push_back(X[i]);
                right_y.push_back(y[i]);
            }
        }

        // Build children recursively
        node->set_left(build_tree(left_X, left_y, depth + 1));
        node->set_right(build_tree(right_X, right_y, depth + 1));

        return node;
    }

    // Compute the importance of a single feature
    double compute_feature_importance(
        size_t feature_idx, const std::vector<std::vector<double>> &X,
        const std::vector<double> &y,
        std::vector<std::vector<double>> &X_permuted) const {

        // Make a copy of X to permute the feature
        X_permuted = X;

        // Permute the feature values
        std::vector<double> feature_values;
        for (const auto &sample : X) {
            if (feature_idx < sample.size()) {
                feature_values.push_back(sample[feature_idx]);
            }
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(feature_values.begin(), feature_values.end(), g);

        for (size_t i = 0; i < X.size(); ++i) {
            if (feature_idx < X_permuted[i].size() &&
                i < feature_values.size()) {
                X_permuted[i][feature_idx] = feature_values[i];
            }
        }

        // Make predictions with permuted feature
        std::vector<double> y_pred_original = predict(X);
        std::vector<double> y_pred_permuted = predict(X_permuted);

        // Calculate performance difference
        double original_error, permuted_error;

        if (task_type == TaskType::REGRESSION) {
            // MSE for regression
            original_error = 0.0;
            permuted_error = 0.0;

            for (size_t i = 0; i < y.size(); ++i) {
                double diff_original = y[i] - y_pred_original[i];
                double diff_permuted = y[i] - y_pred_permuted[i];

                original_error += diff_original * diff_original;
                permuted_error += diff_permuted * diff_permuted;
            }

            original_error /= y.size();
            permuted_error /= y.size();
        } else {
            // Accuracy for classification
            int correct_original = 0;
            int correct_permuted = 0;

            for (size_t i = 0; i < y.size(); ++i) {
                if (std::round(y_pred_original[i]) == y[i]) {
                    correct_original++;
                }
                if (std::round(y_pred_permuted[i]) == y[i]) {
                    correct_permuted++;
                }
            }

            original_error =
                1.0 - static_cast<double>(correct_original) / y.size();
            permuted_error =
                1.0 - static_cast<double>(correct_permuted) / y.size();
        }

        // The importance is the difference in error
        return permuted_error - original_error;
    }

  public:
    // Constructor
    DecisionTree(TaskType task_type = TaskType::REGRESSION,
                 size_t max_depth = std::numeric_limits<size_t>::max(),
                 size_t min_samples_split = 2, size_t max_features = 0,
                 size_t n_classes = 0)
        : root(nullptr), task_type(task_type), max_depth(max_depth),
          min_samples_split(min_samples_split), max_features(max_features),
          n_classes(n_classes) {}

    // Fit the tree to the data
    void fit(const std::vector<std::vector<double>> &X,
             const std::vector<double> &y) {
        if (X.empty() || y.empty() || X.size() != y.size()) {
            return;
        }

        // For classification, determine the number of classes if not provided
        if (task_type == TaskType::CLASSIFICATION && n_classes == 0) {
            double max_class = -1;
            for (double val : y) {
                max_class = std::max(max_class, val);
            }
            n_classes = static_cast<size_t>(max_class) + 1;
        }

        // Set default max_features if not specified
        if (max_features == 0 && !X.empty() && !X[0].empty()) {
            max_features = static_cast<size_t>(std::sqrt(X[0].size()));
        }

        // Build the tree
        root = build_tree(X, y, 0);
    }

    // Predict a single sample
    double predict_sample(const std::vector<double> &features) const {
        if (!root)
            return 0.0;
        return root->predict(features);
    }

    // Predict multiple samples
    std::vector<double>
    predict(const std::vector<std::vector<double>> &X) const {
        std::vector<double> predictions;
        predictions.reserve(X.size());

        for (const auto &features : X) {
            predictions.push_back(predict_sample(features));
        }

        return predictions;
    }

    // Calculate feature importance
    std::vector<double>
    feature_importance(const std::vector<std::vector<double>> &X,
                       const std::vector<double> &y) const {

        if (X.empty() || y.empty() || !root) {
            return std::vector<double>();
        }

        size_t n_features = X[0].size();
        std::vector<double> importance(n_features, 0.0);

        // For each feature, calculate importance
        std::vector<std::vector<double>> X_permuted;
        for (size_t i = 0; i < n_features; ++i) {
            importance[i] = compute_feature_importance(i, X, y, X_permuted);
        }

        // Normalize importance values
        double sum = std::accumulate(importance.begin(), importance.end(), 0.0);
        if (sum > 0) {
            for (double &val : importance) {
                val /= sum;
            }
        }

        return importance;
    }
};

/**
 * @brief Random Forest implementation
 */
// Helper method for bootstrap sampling
inline std::tuple<std::vector<std::vector<double>>, std::vector<double>>
RandomForest::bootstrapSample(const std::vector<std::vector<double>> &X,
                              const std::vector<double> &y) const {
    std::vector<std::vector<double>> X_bootstrap;
    std::vector<double> y_bootstrap;

    size_t n_samples = X.size();

    // Generate random indices with replacement
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> dist(0, n_samples - 1);

    for (size_t i = 0; i < n_samples; ++i) {
        size_t idx = dist(g);
        X_bootstrap.push_back(X[idx]);
        y_bootstrap.push_back(y[idx]);
    }

    return {X_bootstrap, y_bootstrap};
}

// Helper method to extract features and target from dataframe
inline std::tuple<std::vector<std::vector<double>>, std::vector<double>>
RandomForest::extractFeatures(const df::Dataframe &data,
                              const std::string &target_column) {
    std::vector<std::vector<double>> features;
    std::vector<double> target;

    size_t num_samples = 0;
    std::vector<std::string> feature_columns;

    // Identify feature columns (all columns except the target)
    for (const auto &name : data.names()) {
        if (name != target_column) {
            feature_columns.push_back(name);
        }
    }

    if (feature_columns.empty()) {
        throw std::runtime_error("No feature columns found");
    }

    // First pass: determine sizes and handle string target if needed
    if (data.has(target_column)) {
        std::type_index target_type = data.type(target_column);

        // Check if target is a string type
        if (target_type == typeid(df::Serie<std::string>)) {
            has_string_target_ = true;

            // Get string target
            const df::Serie<std::string> &string_target =
                data.get<std::string>(target_column);
            num_samples = string_target.size();

            // Encode the target
            target_encoder_.fit(string_target);

            // If classification, update n_classes based on number of unique
            // values
            if (task_type == TaskType::CLASSIFICATION && n_classes == 0) {
                n_classes = target_encoder_.num_categories();
            }

            // Transform target to numeric values
            df::Serie<double> encoded_target =
                target_encoder_.transform(string_target);
            target = encoded_target.data();
        } else {
            // Handle numeric target types
            has_string_target_ = false;

            if (target_type == typeid(df::Serie<int>)) {
                const df::Serie<int> &int_target = data.get<int>(target_column);
                num_samples = int_target.size();
                target = convertToDoubleVector(int_target.data());
            } else if (target_type == typeid(df::Serie<double>)) {
                const df::Serie<double> &double_target =
                    data.get<double>(target_column);
                num_samples = double_target.size();
                target = double_target.data();
            } else {
                throw std::runtime_error("Unsupported target type");
            }
        }
    } else {
        throw std::runtime_error("Target column not found: " + target_column);
    }

    // Initialize features array
    features.resize(num_samples);

    // Process each feature column
    for (const auto &col_name : feature_columns) {
        feature_names_.push_back(col_name);

        std::type_index feature_type = data.type(col_name);

        if (feature_type == typeid(df::Serie<std::string>)) {
            // Handle string feature
            const df::Serie<std::string> &string_feature =
                data.get<std::string>(col_name);

            // Create and fit encoder for this feature
            df::LabelEncoder encoder;
            encoder.fit(string_feature);
            feature_encoders_[col_name] = encoder;

            // Transform feature to numeric values
            df::Serie<double> encoded_feature =
                encoder.transform(string_feature);

            // Add to features matrix
            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(encoded_feature[i]);
            }
        } else if (feature_type == typeid(df::Serie<int>)) {
            // Handle integer feature
            const df::Serie<int> &int_feature = data.get<int>(col_name);
            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(static_cast<double>(int_feature[i]));
            }
        } else if (feature_type == typeid(df::Serie<double>)) {
            // Handle double feature
            const df::Serie<double> &double_feature =
                data.get<double>(col_name);
            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(double_feature[i]);
            }
        } else {
            throw std::runtime_error("Unsupported feature type for column: " +
                                     col_name);
        }
    }

    return {features, target};
}

// Helper method to extract features for prediction (no target column)
inline std::vector<std::vector<double>>
RandomForest::extractFeaturesForPrediction(const df::Dataframe &data) const {
    if (feature_names_.empty()) {
        throw std::runtime_error(
            "Model not fitted. Call fit() before predict()");
    }

    size_t num_samples = 0;
    bool size_initialized = false;

    // Initialize features array
    std::vector<std::vector<double>> features;

    // Process each feature column in the same order as during training
    for (const auto &col_name : feature_names_) {
        if (!data.has(col_name)) {
            throw std::runtime_error("Feature column not found: " + col_name);
        }

        std::type_index feature_type = data.type(col_name);

        if (feature_type == typeid(df::Serie<std::string>)) {
            // Handle string feature
            const df::Serie<std::string> &string_feature =
                data.get<std::string>(col_name);

            if (!size_initialized) {
                num_samples = string_feature.size();
                features.resize(num_samples);
                size_initialized = true;
            }

            // Check if we have an encoder for this feature
            auto encoder_it = feature_encoders_.find(col_name);
            if (encoder_it == feature_encoders_.end()) {
                throw std::runtime_error(
                    "No encoder found for string feature: " + col_name);
            }

            // Transform feature to numeric values
            df::Serie<double> encoded_feature =
                encoder_it->second.transform(string_feature);

            // Add to features matrix
            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(encoded_feature[i]);
            }
        } else if (feature_type == typeid(df::Serie<int>)) {
            // Handle integer feature
            const df::Serie<int> &int_feature = data.get<int>(col_name);

            if (!size_initialized) {
                num_samples = int_feature.size();
                features.resize(num_samples);
                size_initialized = true;
            }

            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(static_cast<double>(int_feature[i]));
            }
        } else if (feature_type == typeid(df::Serie<double>)) {
            // Handle double feature
            const df::Serie<double> &double_feature =
                data.get<double>(col_name);

            if (!size_initialized) {
                num_samples = double_feature.size();
                features.resize(num_samples);
                size_initialized = true;
            }

            for (size_t i = 0; i < num_samples; ++i) {
                features[i].push_back(double_feature[i]);
            }
        } else {
            throw std::runtime_error("Unsupported feature type for column: " +
                                     col_name);
        }
    }

    return features;
}

// Helper to convert df::Serie to std::vector
template <typename T>
inline std::vector<double>
RandomForest::convertToDoubleVector(const std::vector<T> &input) const {
    std::vector<double> result;
    result.reserve(input.size());

    for (const auto &val : input) {
        result.push_back(static_cast<double>(val));
    }

    return result;
}

// Compute out-of-bag (OOB) samples for a bootstrap sample
inline std::vector<size_t>
RandomForest::computeOOBSamples(const std::vector<size_t> &bootstrap_indices,
                                size_t n_samples) const {

    std::unordered_set<size_t> bootstrap_set(bootstrap_indices.begin(),
                                             bootstrap_indices.end());
    std::vector<size_t> oob_indices;

    for (size_t i = 0; i < n_samples; ++i) {
        if (bootstrap_set.find(i) == bootstrap_set.end()) {
            oob_indices.push_back(i);
        }
    }

    return oob_indices;
}

inline RandomForest::RandomForest(size_t num_trees, TaskType task_type,
                                  size_t max_features, size_t max_depth,
                                  size_t min_samples_split, size_t n_classes)
    : task_type(task_type), num_trees(num_trees), max_features(max_features),
      max_depth(max_depth), min_samples_split(min_samples_split),
      n_classes(n_classes) {
    trees.resize(num_trees,
                 DecisionTree(task_type, max_depth, min_samples_split,
                              max_features, n_classes));
}

inline void RandomForest::fit(const df::Dataframe &data,
                              const std::string &target_column) {
    // Extract features and target
    auto [features, target] = extractFeatures(data, target_column);

    // For classification, determine the number of classes if not provided
    if (task_type == TaskType::CLASSIFICATION && n_classes == 0) {
        double max_class = -1;
        for (double val : target) {
            max_class = std::max(max_class, val);
        }
        n_classes = static_cast<size_t>(max_class) + 1;

        // Update n_classes in all trees
        for (auto &tree : trees) {
            tree = DecisionTree(task_type, max_depth, min_samples_split,
                                max_features, n_classes);
        }
    }

    // Default max_features if not specified
    if (max_features == 0 && !features.empty() && !features[0].empty()) {
        max_features = static_cast<size_t>(std::sqrt(features[0].size()));

        // Update max_features in all trees
        for (auto &tree : trees) {
            tree = DecisionTree(task_type, max_depth, min_samples_split,
                                max_features, n_classes);
        }
    }

// Train each tree with bootstrap samples
#pragma omp parallel for
    for (size_t i = 0; i < num_trees; i++) {
        // Create bootstrap sample
        auto [bootstrap_features, bootstrap_target] =
            bootstrapSample(features, target);

        // Train individual tree
        trees[i].fit(bootstrap_features, bootstrap_target);
    }
}

inline df::Serie<double> RandomForest::predict(const df::Dataframe &data) {
    // Extract features
    auto features = extractFeaturesForPrediction(data);

    // Get predictions from all trees
    std::vector<std::vector<double>> all_predictions(num_trees);

#pragma omp parallel for
    for (size_t i = 0; i < num_trees; i++) {
        all_predictions[i] = trees[i].predict(features);
    }

    // Aggregate predictions
    std::vector<double> final_predictions(features.size(), 0.0);

    if (task_type == TaskType::REGRESSION) {
        // Average predictions for regression
        for (size_t i = 0; i < features.size(); i++) {
            double sum = 0.0;
            for (size_t t = 0; t < num_trees; t++) {
                if (i < all_predictions[t].size()) {
                    sum += all_predictions[t][i];
                }
            }
            final_predictions[i] = sum / num_trees;
        }
    } else {
        // Majority voting for classification
        for (size_t i = 0; i < features.size(); i++) {
            std::vector<size_t> class_votes(n_classes, 0);

            for (size_t t = 0; t < num_trees; t++) {
                if (i < all_predictions[t].size()) {
                    size_t predicted_class =
                        static_cast<size_t>(std::round(all_predictions[t][i]));
                    if (predicted_class < n_classes) {
                        class_votes[predicted_class]++;
                    }
                }
            }

            // Find majority class
            size_t majority_class = 0;
            size_t max_votes = 0;

            for (size_t c = 0; c < n_classes; c++) {
                if (class_votes[c] > max_votes) {
                    max_votes = class_votes[c];
                    majority_class = c;
                }
            }

            final_predictions[i] = static_cast<double>(majority_class);
        }
    }

    return df::Serie<double>(final_predictions);
}

inline df::Serie<double>
RandomForest::feature_importance(const df::Dataframe &data,
                                 const std::string &target_column) {
    // Extract features and target
    auto [features, target] = extractFeatures(data, target_column);

    if (features.empty() || features[0].empty()) {
        return df::Serie<double>();
    }

    size_t n_features = features[0].size();
    std::vector<double> importance(n_features, 0.0);

// Calculate importance for each tree and average
#pragma omp parallel for
    for (size_t i = 0; i < num_trees; i++) {
        std::vector<double> tree_importance =
            trees[i].feature_importance(features, target);

#pragma omp critical
        {
            for (size_t j = 0; j < n_features && j < tree_importance.size();
                 j++) {
                importance[j] += tree_importance[j] / num_trees;
            }
        }
    }

    // Normalize importance values to sum to 1
    double sum = std::accumulate(importance.begin(), importance.end(), 0.0);
    if (sum > 0) {
        for (double &val : importance) {
            val /= sum;
        }
    }

    return df::Serie<double>(importance);
}

// Return string predictions when target was string type
df::Serie<std::string> RandomForest::predict_categorical(const df::Dataframe& data) {
    if (!has_string_target_) {
        throw std::runtime_error("Target was not a string type. Use predict() instead.");
    }
    
    // Get numeric predictions
    df::Serie<double> numeric_predictions = predict(data);
    
    // Convert back to original string labels
    return target_encoder_.inverse_transform(numeric_predictions);
}

// Calculate out-of-bag (OOB) error
inline double RandomForest::oob_error(const df::Dataframe &data,
                                      const std::string &target_column) {
    // Extract features and target
    auto [features, target] = extractFeatures(data, target_column);

    if (features.empty()) {
        return 0.0;
    }

    size_t n_samples = features.size();

    // Count how many times each sample is used in OOB evaluation
    std::vector<size_t> oob_counts(n_samples, 0);

    // For each sample, accumulate predictions from trees where it's OOB
    std::vector<double> oob_predictions(n_samples, 0.0);

    // For classification, we need to count votes for each class
    std::vector<std::vector<size_t>> class_votes;
    if (task_type == TaskType::CLASSIFICATION) {
        class_votes.resize(n_samples, std::vector<size_t>(n_classes, 0));
    }

    // Generate bootstrap samples and compute OOB predictions
    for (size_t i = 0; i < num_trees; i++) {
        // Generate bootstrap indices
        std::vector<size_t> bootstrap_indices;
        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<size_t> dist(0, n_samples - 1);

        for (size_t j = 0; j < n_samples; j++) {
            bootstrap_indices.push_back(dist(g));
        }

        // Compute OOB samples
        std::vector<size_t> oob_indices =
            computeOOBSamples(bootstrap_indices, n_samples);

        if (oob_indices.empty()) {
            continue;
        }

        // Extract OOB features for prediction
        std::vector<std::vector<double>> oob_features;
        for (size_t idx : oob_indices) {
            if (idx < features.size()) {
                oob_features.push_back(features[idx]);
            }
        }

        // Predict OOB samples
        std::vector<double> tree_predictions = trees[i].predict(oob_features);

        // Accumulate predictions
        for (size_t j = 0;
             j < oob_indices.size() && j < tree_predictions.size(); j++) {
            size_t idx = oob_indices[j];

            if (task_type == TaskType::REGRESSION) {
                oob_predictions[idx] += tree_predictions[j];
            } else {
                size_t predicted_class =
                    static_cast<size_t>(std::round(tree_predictions[j]));
                if (predicted_class < n_classes) {
                    class_votes[idx][predicted_class]++;
                }
            }

            oob_counts[idx]++;
        }
    }

    // Calculate error
    double error = 0.0;
    size_t valid_samples = 0;

    for (size_t i = 0; i < n_samples; i++) {
        if (oob_counts[i] > 0) {
            if (task_type == TaskType::REGRESSION) {
                // Average prediction for regression
                double prediction = oob_predictions[i] / oob_counts[i];
                double diff = target[i] - prediction;
                error += diff * diff;
            } else {
                // Majority vote for classification
                size_t max_votes = 0;
                size_t predicted_class = 0;

                for (size_t c = 0; c < n_classes; c++) {
                    if (class_votes[i][c] > max_votes) {
                        max_votes = class_votes[i][c];
                        predicted_class = c;
                    }
                }

                if (static_cast<size_t>(target[i]) != predicted_class) {
                    error += 1.0;
                }
            }

            valid_samples++;
        }
    }

    if (valid_samples > 0) {
        if (task_type == TaskType::REGRESSION) {
            // MSE for regression
            error /= valid_samples;
        } else {
            // Error rate for classification
            error /= valid_samples;
        }
    }

    return error;
}

// Calculate variable importance using permutation importance method
inline df::Serie<double>
RandomForest::permutation_importance(const df::Dataframe &data,
                                     const std::string &target_column,
                                     size_t n_repeats) {
    // Extract features and target
    auto [features, target] = extractFeatures(data, target_column);

    if (features.empty() || features[0].empty()) {
        return df::Serie<double>();
    }

    size_t n_features = features[0].size();
    std::vector<double> importance(n_features, 0.0);
    std::vector<double> importance_std(n_features, 0.0);

    // Make baseline predictions
    std::vector<double> y_pred;
    for (const auto &tree : trees) {
        std::vector<double> tree_preds = tree.predict(features);

        if (y_pred.empty()) {
            y_pred = tree_preds;
        } else {
            for (size_t i = 0; i < y_pred.size() && i < tree_preds.size();
                 i++) {
                y_pred[i] += tree_preds[i];
            }
        }
    }

    // Average predictions
    for (double &pred : y_pred) {
        pred /= num_trees;
    }

    // Calculate baseline score
    double baseline_score;
    if (task_type == TaskType::REGRESSION) {
        // MSE for regression
        baseline_score = 0.0;
        for (size_t i = 0; i < target.size() && i < y_pred.size(); i++) {
            double diff = target[i] - y_pred[i];
            baseline_score += diff * diff;
        }
        baseline_score /= target.size();
    } else {
        // Accuracy for classification
        baseline_score = 0.0;
        for (size_t i = 0; i < target.size() && i < y_pred.size(); i++) {
            if (static_cast<size_t>(std::round(y_pred[i])) ==
                static_cast<size_t>(target[i])) {
                baseline_score += 1.0;
            }
        }
        baseline_score /= target.size();
    }

// Calculate permutation importance
#pragma omp parallel for
    for (size_t i = 0; i < n_features; i++) {
        std::vector<double> scores(n_repeats);

        for (size_t r = 0; r < n_repeats; r++) {
            // Create a copy of features
            std::vector<std::vector<double>> X_permuted = features;

            // Permute the feature
            std::vector<double> feature_values;
            for (const auto &sample : features) {
                if (i < sample.size()) {
                    feature_values.push_back(sample[i]);
                }
            }

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(feature_values.begin(), feature_values.end(), g);

            for (size_t j = 0;
                 j < X_permuted.size() && j < feature_values.size(); j++) {
                if (i < X_permuted[j].size()) {
                    X_permuted[j][i] = feature_values[j];
                }
            }

            // Make predictions with permuted feature
            std::vector<double> y_pred_permuted;
            for (const auto &tree : trees) {
                std::vector<double> tree_preds = tree.predict(X_permuted);

                if (y_pred_permuted.empty()) {
                    y_pred_permuted = tree_preds;
                } else {
                    for (size_t j = 0;
                         j < y_pred_permuted.size() && j < tree_preds.size();
                         j++) {
                        y_pred_permuted[j] += tree_preds[j];
                    }
                }
            }

            // Average predictions
            for (double &pred : y_pred_permuted) {
                pred /= num_trees;
            }

            // Calculate permuted score
            double permuted_score;
            if (task_type == TaskType::REGRESSION) {
                // MSE for regression
                permuted_score = 0.0;
                for (size_t j = 0;
                     j < target.size() && j < y_pred_permuted.size(); j++) {
                    double diff = target[j] - y_pred_permuted[j];
                    permuted_score += diff * diff;
                }
                permuted_score /= target.size();

                // For regression, increase in error is more important
                scores[r] = permuted_score - baseline_score;
            } else {
                // Accuracy for classification
                permuted_score = 0.0;
                for (size_t j = 0;
                     j < target.size() && j < y_pred_permuted.size(); j++) {
                    if (static_cast<size_t>(std::round(y_pred_permuted[j])) ==
                        static_cast<size_t>(target[j])) {
                        permuted_score += 1.0;
                    }
                }
                permuted_score /= target.size();

                // For classification, decrease in accuracy is more
                // important
                scores[r] = baseline_score - permuted_score;
            }
        }

        // Calculate mean and std of importance scores
        double mean =
            std::accumulate(scores.begin(), scores.end(), 0.0) / n_repeats;

        double variance = 0.0;
        for (double score : scores) {
            double diff = score - mean;
            variance += diff * diff;
        }
        double std_dev = std::sqrt(variance / n_repeats);

#pragma omp critical
        {
            importance[i] = mean;
            importance_std[i] = std_dev;
        }
    }

    // Return importance values
    return df::Serie<double>(importance);
}

// Get the feature names from the dataframe, excluding the target column
inline std::vector<std::string>
RandomForest::get_feature_names(const df::Dataframe &data,
                                const std::string &target_column) const {
    std::vector<std::string> feature_names;

    for (const auto &name : data.names()) {
        if (name != target_column) {
            feature_names.push_back(name);
        }
    }

    return feature_names;
}

// Create a dataframe with feature names and their importance values
inline df::Dataframe
RandomForest::feature_importance_df(const df::Dataframe &data,
                                    const std::string &target_column) {
    df::Dataframe result;

    // Get feature names
    auto feature_names = get_feature_names(data, target_column);

    // Calculate feature importance
    auto importance = feature_importance(data, target_column);

    // Create a Serie of feature names
    df::Serie<std::string> names_serie(feature_names);

    // Add to dataframe
    result.add("feature", names_serie);
    result.add("importance", importance);

    return result;
}

// Calculate model performance metrics
inline std::map<std::string, double>
RandomForest::evaluate(const df::Dataframe &data,
                       const std::string &target_column) {
    std::map<std::string, double> metrics;

    // Extract features and target
    auto [features, target] = extractFeatures(data, target_column);

    // Make predictions
    auto pred_serie = predict(data);
    std::vector<double> predictions = pred_serie.data();

    if (task_type == TaskType::REGRESSION) {
        // Calculate regression metrics
        double mse = 0.0, mae = 0.0, r2 = 0.0;
        double target_mean =
            std::accumulate(target.begin(), target.end(), 0.0) / target.size();
        double ss_total = 0.0, ss_residual = 0.0;

        for (size_t i = 0; i < target.size() && i < predictions.size(); i++) {
            double diff = target[i] - predictions[i];
            mse += diff * diff;
            mae += std::abs(diff);

            double target_diff = target[i] - target_mean;
            ss_total += target_diff * target_diff;
            ss_residual += diff * diff;
        }

        size_t n = std::min(target.size(), predictions.size());
        if (n > 0) {
            mse /= n;
            mae /= n;

            // R^2 = 1 - SS_residual / SS_total
            r2 = (ss_total > 0) ? 1.0 - (ss_residual / ss_total) : 0.0;
        }

        metrics["mse"] = mse;
        metrics["rmse"] = std::sqrt(mse);
        metrics["mae"] = mae;
        metrics["r2"] = r2;
    } else {
        // Calculate classification metrics
        double accuracy = 0.0;

        // For binary classification, also calculate precision, recall, f1
        bool is_binary = (n_classes == 2);
        double tp = 0.0, fp = 0.0, tn = 0.0, fn = 0.0;

        for (size_t i = 0; i < target.size() && i < predictions.size(); i++) {
            size_t true_class = static_cast<size_t>(target[i]);
            size_t pred_class = static_cast<size_t>(std::round(predictions[i]));

            // Accuracy
            if (true_class == pred_class) {
                accuracy += 1.0;
            }

            // For binary classification
            if (is_binary) {
                if (true_class == 1 && pred_class == 1)
                    tp += 1.0;
                if (true_class == 0 && pred_class == 1)
                    fp += 1.0;
                if (true_class == 0 && pred_class == 0)
                    tn += 1.0;
                if (true_class == 1 && pred_class == 0)
                    fn += 1.0;
            }
        }

        size_t n = std::min(target.size(), predictions.size());
        if (n > 0) {
            accuracy /= n;
        }

        metrics["accuracy"] = accuracy;

        if (is_binary) {
            double precision = (tp + fp > 0) ? tp / (tp + fp) : 0.0;
            double recall = (tp + fn > 0) ? tp / (tp + fn) : 0.0;
            double f1 = (precision + recall > 0)
                            ? 2.0 * precision * recall / (precision + recall)
                            : 0.0;

            metrics["precision"] = precision;
            metrics["recall"] = recall;
            metrics["f1"] = f1;
        }
    }

    return metrics;
};

// Utility function to create a Random Forest Regressor
inline RandomForest create_random_forest_regressor(size_t num_trees,
                                                   size_t max_features,
                                                   size_t max_depth,
                                                   size_t min_samples_split) {

    return RandomForest(num_trees, TaskType::REGRESSION, max_features,
                        max_depth, min_samples_split);
}

// Utility function to create a Random Forest Classifier
inline RandomForest create_random_forest_classifier(size_t num_trees,
                                                    size_t n_classes,
                                                    size_t max_features,
                                                    size_t max_depth,
                                                    size_t min_samples_split) {

    return RandomForest(num_trees, TaskType::CLASSIFICATION, max_features,
                        max_depth, min_samples_split, n_classes);
}

} // namespace ml
