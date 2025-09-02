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

#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/core/filter.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/core/zip.h>
#include <dataframe/io/csv.h>
#include <dataframe/ml/lime.h>
#include <dataframe/ml/random_forest.h>
#include <iomanip>
#include <iostream>
#include <string>

// Helper function to get a single row from a Dataframe
df::Dataframe get_dataframe_row(const df::Dataframe& df, size_t row_index)
{
    df::Dataframe result;

    for (const auto& col_name : df.names()) {
        if (df.type_name(col_name) == "double") {
            auto value = df.get<double>(col_name)[row_index];
            result.add(col_name, df::Serie<double> { value });
        } else if (df.type_name(col_name) == "int") {
            auto value = df.get<int>(col_name)[row_index];
            result.add(col_name, df::Serie<int> { value });
        } else if (df.type_name(col_name) == "string") {
            auto value = df.get<std::string>(col_name)[row_index];
            result.add(col_name, df::Serie<std::string> { value });
        }
    }

    return result;
}

int main(int argc, char* argv[])
{
    std::string filename = "iris.csv";

    // Allow user to specify a different filename
    if (argc > 1) {
        filename = argv[1];
    }

    try {
        // Load sample dataset (e.g., iris or your own dataset)
        std::cout << "Loading dataset..." << std::endl;
        df::Dataframe data = df::io::read_csv(filename);

        // Set the target column
        std::string target_column = "species";

        // Define categorical features (for the iris dataset)
        std::set<std::string> categorical_features;
        categorical_features.insert("species");

        // Split data into training and test sets (80/20 split)
        std::cout << "Splitting data into train/test sets..." << std::endl;
        size_t total_rows = data.get<double>(data.names()[0]).size();
        size_t train_size = static_cast<size_t>(0.8 * total_rows);

        df::Dataframe train_data;
        df::Dataframe test_data;

        // Create training data
        for (const auto& col_name : data.names()) {
            // MSG(data.type_name(col_name));
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

        // Determine number of classes for the classifier
        size_t n_classes = 0;
        if (train_data.has(target_column)) {
            auto target_values = train_data.get<std::string>(target_column);
            std::set<std::string> unique_values;
            for (size_t i = 0; i < target_values.size(); ++i) {
                unique_values.insert(target_values[i]);
            }
            n_classes = unique_values.size();
        }

        std::cout << "Training Random Forest classifier with " << n_classes << " classes..."
                  << std::endl;

        // Create and train a random forest classifier
        ml::RandomForest rf =
            // num_trees, n_classes, maxfeatures (auto), max_depth,
            // min_samples_split
            ml::create_random_forest_classifier(100, n_classes, 0, 10, 2);

        rf.fit(train_data, target_column);

        // Make predictions on test data
        df::Serie<double> predictions = rf.predict(test_data);

        // Calculate accuracy
        auto test_targets = test_data.get<std::string>(target_column);
        size_t correct = 0;

        for (size_t i = 0; i < predictions.size(); ++i) {
            int pred_class = static_cast<int>(predictions[i]);

            // Get the class name from the integer prediction
            std::string pred_class_name;
            auto target_values = train_data.get<std::string>(target_column);
            std::set<std::string> unique_values;
            for (size_t j = 0; j < target_values.size(); ++j) {
                unique_values.insert(target_values[j]);
            }

            std::vector<std::string> class_names(unique_values.begin(), unique_values.end());
            if (pred_class >= 0 && pred_class < static_cast<int>(class_names.size())) {
                pred_class_name = class_names[pred_class];
            }

            if (pred_class_name == test_targets[i]) {
                correct++;
            }
        }

        double accuracy = static_cast<double>(correct) / predictions.size();
        std::cout << "Random Forest accuracy: " << std::fixed << std::setprecision(4)
                  << accuracy * 100.0 << "%" << std::endl;

        // Create LIME explainer
        std::cout << "Creating LIME explainer..." << std::endl;
        ml::Lime lime_explainer(train_data, target_column, categorical_features, 0.75, true);

        // Choose an instance to explain
        size_t instance_idx = 0; // Choose the first test instance
        df::Dataframe instance_to_explain = get_dataframe_row(test_data, instance_idx);

        // Display the instance
        std::cout << "\nExplaining instance:\n";
        for (const auto& col_name : instance_to_explain.names()) {
            // MSG("col_name: ",col_name);
            if (col_name != target_column) {
                if (instance_to_explain.type_name(col_name) == "double") {
                    std::cout << "  " << col_name << ": "
                              << instance_to_explain.get<double>(col_name)[0] << std::endl;
                } else if (instance_to_explain.type_name(col_name) == "string") {
                    std::cout << "  " << col_name << ": "
                              << instance_to_explain.get<std::string>(col_name)[0] << std::endl;
                }
            }
        }

        std::cout << "  " << target_column << ": "
                  << instance_to_explain.get<std::string>(target_column)[0] << std::endl;

        // Create a prediction function for the random forest
        auto predict_fn = [&rf](const df::Dataframe& samples) { return rf.predict(samples); };

        // Get the explanation
        std::cout << "\nGenerating explanation..." << std::endl;
        auto explanation = lime_explainer.explain(instance_to_explain, predict_fn,
            5, // Number of features to include in explanation
            1000 // Number of samples to generate
        );

        // Display the explanation
        std::cout << "\nLIME Explanation for the prediction:" << std::endl;
        std::cout << "Feature                 Weight" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        for (const auto& [feature, weight] : explanation) {
            std::cout << std::left << std::setw(24) << feature << std::right << std::setw(10)
                      << std::fixed << std::setprecision(4) << weight << std::endl;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}