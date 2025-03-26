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

#include "../../TEST.h"
#include <algorithm>
#include <cmath>
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/ml/lime.h>
#include <dataframe/ml/random_forest.h>
#include <random>
#include <set>
#include <string>
#include <vector>

// Helper function to create a synthetic dataset
df::Dataframe create_synthetic_dataset(size_t n_samples = 1000) {
    df::Dataframe data;

    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::normal_distribution<double> normal_dist(0.0, 1.0);
    std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);

    // Create features
    std::vector<double> feature1, feature2, feature3, feature4;
    std::vector<std::string> categorical;

    // Create categorical values
    std::vector<std::string> categories = {"A", "B", "C"};

    feature1.reserve(n_samples);
    feature2.reserve(n_samples);
    feature3.reserve(n_samples);
    feature4.reserve(n_samples);
    categorical.reserve(n_samples);

    for (size_t i = 0; i < n_samples; ++i) {
        feature1.push_back(normal_dist(rng));
        feature2.push_back(normal_dist(rng));
        feature3.push_back(normal_dist(rng));
        feature4.push_back(uniform_dist(rng));

        // Assign categorical value based on uniform distribution
        size_t cat_idx =
            static_cast<size_t>(uniform_dist(rng) * categories.size());
        if (cat_idx >= categories.size())
            cat_idx = categories.size() - 1;
        categorical.push_back(categories[cat_idx]);
    }

    // Add features to the dataframe
    data.add("feature1", df::Serie<double>(feature1));
    data.add("feature2", df::Serie<double>(feature2));
    data.add("feature3", df::Serie<double>(feature3));
    data.add("feature4", df::Serie<double>(feature4));
    data.add("categorical", df::Serie<std::string>(categorical));

    // Create target: binary classification based on a nonlinear function of
    // features
    std::vector<std::string> target;
    target.reserve(n_samples);

    for (size_t i = 0; i < n_samples; ++i) {
        // Decision function: feature1^2 + feature2 - 2*feature3 + (categorical
        // == "A" ? 1 : 0)
        double decision = feature1[i] * feature1[i] + feature2[i] -
                          2.0 * feature3[i] +
                          (categorical[i] == "A" ? 1.0 : 0.0);

        // Add some noise
        decision += 0.1 * normal_dist(rng);

        // Classify based on decision value
        if (decision > 0.0) {
            target.push_back("positive");
        } else {
            target.push_back("negative");
        }
    }

    // Add target to dataframe
    data.add("target", df::Serie<std::string>(target));

    return data;
}

// Helper function to get a single row from a Dataframe
df::Dataframe get_dataframe_row(const df::Dataframe &df, size_t row_index) {
    df::Dataframe result;

    for (const auto &col_name : df.names()) {
        if (df.type_name(col_name) == "double") {
            auto value = df.get<double>(col_name)[row_index];
            result.add(col_name, df::Serie<double>{value});
        } else if (df.type_name(col_name) == "int") {
            auto value = df.get<int>(col_name)[row_index];
            result.add(col_name, df::Serie<int>{value});
        } else if (df.type_name(col_name) == "string") {
            auto value = df.get<std::string>(col_name)[row_index];
            result.add(col_name, df::Serie<std::string>{value});
        }
    }

    return result;
}

// Test LIME with a Random Forest classifier
TEST(LIME, RandomForestClassifier) {
    // Create synthetic dataset
    df::Dataframe data = create_synthetic_dataset(1000);

    MSG("Created synthetic dataset with ", data.get<double>("feature1").size(),
        " samples");

    // Define categorical features
    std::set<std::string> categorical_features;
    categorical_features.insert("categorical");
    categorical_features.insert("target");

    // Split data into training and test sets (80/20 split)
    size_t total_rows = data.get<double>("feature1").size();
    size_t train_size = static_cast<size_t>(0.8 * total_rows);

    df::Dataframe train_data;
    df::Dataframe test_data;

    // Create training data
    for (const auto &col_name : data.names()) {
        if (data.type_name(col_name) == "double") {
            auto full_col = data.get<double>(col_name);
            std::vector<double> train_values(
                full_col.data().begin(), full_col.data().begin() + train_size);
            std::vector<double> test_values(
                full_col.data().begin() + train_size, full_col.data().end());

            train_data.add(col_name, df::Serie<double>(train_values));
            test_data.add(col_name, df::Serie<double>(test_values));
        } else if (data.type_name(col_name) == "string") {
            auto full_col = data.get<std::string>(col_name);
            std::vector<std::string> train_values(
                full_col.data().begin(), full_col.data().begin() + train_size);
            std::vector<std::string> test_values(
                full_col.data().begin() + train_size, full_col.data().end());

            train_data.add(col_name, df::Serie<std::string>(train_values));
            test_data.add(col_name, df::Serie<std::string>(test_values));
        }
    }

    MSG("Split data into ", train_data.get<double>("feature1").size(),
        " training samples and ", test_data.get<double>("feature1").size(),
        " test samples");

    // Determine number of classes for the classifier
    size_t n_classes = 0;
    if (train_data.has("target")) {
        auto target_values = train_data.get<std::string>("target");
        std::set<std::string> unique_values;
        for (size_t i = 0; i < target_values.size(); ++i) {
            unique_values.insert(target_values[i]);
        }
        n_classes = unique_values.size();
    }

    MSG("Training Random Forest classifier with ", n_classes, " classes");

    // Create and train a random forest classifier
    ml::RandomForest rf =
        ml::create_random_forest_classifier(100,       // num_trees
                                            n_classes, // n_classes
                                            0,         // max_features (auto)
                                            10,        // max_depth
                                            2          // min_samples_split
        );

    rf.fit(train_data, "target");

    // Make predictions on test data
    df::Serie<double> predictions = rf.predict(test_data);

    // Calculate accuracy
    auto test_targets = test_data.get<std::string>("target");
    size_t correct = 0;

    for (size_t i = 0; i < predictions.size(); ++i) {
        int pred_class = static_cast<int>(predictions[i]);

        // Get the class name from the integer prediction
        std::string pred_class_name;
        auto target_values = train_data.get<std::string>("target");
        std::set<std::string> unique_values;
        for (size_t j = 0; j < target_values.size(); ++j) {
            unique_values.insert(target_values[j]);
        }

        std::vector<std::string> class_names(unique_values.begin(),
                                             unique_values.end());
        if (pred_class >= 0 &&
            pred_class < static_cast<int>(class_names.size())) {
            pred_class_name = class_names[pred_class];
        }

        if (pred_class_name == test_targets[i]) {
            correct++;
        }
    }

    double accuracy = static_cast<double>(correct) / predictions.size();
    MSG("Random Forest accuracy: ", (accuracy * 100.0), "%");

    // Ensure the model has reasonable accuracy
    CHECK(accuracy > 0.7);

    // Create LIME explainer
    MSG("Creating LIME explainer");
    ml::Lime lime_explainer(train_data, "target", categorical_features);

    // Choose an instance to explain
    size_t instance_idx = 0; // Choose the first test instance
    df::Dataframe instance_to_explain =
        get_dataframe_row(test_data, instance_idx);

    // Create a prediction function for the random forest
    auto predict_fn = [&rf](const df::Dataframe &samples) {
        return rf.predict(samples);
    };

    // Get the explanation
    MSG("Generating explanation");
    auto explanation = lime_explainer.explain(
        instance_to_explain, predict_fn,
        5,   // Number of features to include in explanation
        1000 // Number of samples to generate
    );

    // Display the explanation
    MSG("LIME Explanation:");
    for (const auto &[feature, weight] : explanation) {
        MSG("  ", feature, ": ", weight);
    }

    // Verify the explanation has the expected number of features
    CHECK(explanation.size() <= 5);

    // Verify that feature1 is important (since it's squared in our decision
    // function)
    bool feature1_found = false;
    for (const auto &[feature, weight] : explanation) {
        if (feature == "feature1") {
            feature1_found = true;
            break;
        }
    }

    CHECK(feature1_found);
}

// Run the tests
RUN_TESTS()