/**
 * This example demonstrates how to use the Random Forest implementation
 * to predict wine quality based on physiochemical properties.
 *
 * The dataset used is the Wine Quality dataset from UCI Machine Learning
 * Repository. It contains various features like acidity, pH, alcohol content,
 * etc., and the quality rating of different wines on a scale of 0-10.
 */

#include <algorithm>
#include <chrono>
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/core/filter.h>
#include <dataframe/core/map.h>
#include <dataframe/core/reduce.h>
#include <dataframe/core/split.h>
#include <dataframe/core/zip.h>
#include <dataframe/io/csv.h>
#include <dataframe/math/random.h>
#include <dataframe/ml/random_forest.h>
#include <dataframe/stats/stats.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

// Function to load and preprocess the wine quality dataset
df::Dataframe load_wine_data(const std::string &filename) {
    // Load CSV data
    std::cout << "Loading wine quality dataset from " << filename << "..."
              << std::endl;
    df::Dataframe wine_data = df::io::read_csv(filename, df::io::CSVOptions({
                                                             .delimiter = ',',
                                                             .has_header = true,
                                                             .all_double = true,
                                                         }));

    std::cout << "Dataset contains " << wine_data.size() << " columns and ";

    // Get any column to determine number of rows
    if (wine_data.size() > 0) {
        auto first_column = wine_data.names()[0];
        std::cout << wine_data.get<double>(first_column).size() << " rows."
                  << std::endl;
    } else {
        std::cout << "0 rows (empty dataset)." << std::endl;
        return wine_data;
    }

    // Check for missing values
    std::cout << "Checking for missing values..." << std::endl;
    bool has_missing = false;

    for (const auto &col_name : wine_data.names()) {
        try {
            auto column = wine_data.get<double>(col_name);

            // Count NaN or infinite values
            size_t nan_count = 0;
            column.forEach([&nan_count](double value, size_t) {
                if (std::isnan(value) || std::isinf(value)) {
                    nan_count++;
                }
            });

            if (nan_count > 0) {
                std::cout << "  - Column '" << col_name << "' has " << nan_count
                          << " missing values." << std::endl;
                has_missing = true;
            }
        } catch (const std::exception &e) {
            // Column might not be numeric
            std::cout << "  - Column '" << col_name
                      << "' is not numeric or had an error: " << e.what()
                      << std::endl;
        }
    }

    if (!has_missing) {
        std::cout << "No missing values found." << std::endl;
    }

    // Print dataset summary
    std::cout << "\nDataset summary:" << std::endl;
    std::cout << "-----------------" << std::endl;

    for (const auto &col_name : wine_data.names()) {
        try {
            auto column = wine_data.get<double>(col_name);
            auto stats = df::stats::summary(column);

            std::cout << std::left << std::setw(20) << col_name
                      << " | Min: " << std::setw(8) << std::fixed
                      << std::setprecision(2) << stats["min"]
                      << " | Max: " << std::setw(8) << std::fixed
                      << std::setprecision(2) << stats["max"]
                      << " | Mean: " << std::setw(8) << std::fixed
                      << std::setprecision(2) << stats["mean"]
                      << " | Std: " << std::setw(8) << std::fixed
                      << std::setprecision(2) << stats["std_dev"] << std::endl;
        } catch (const std::exception &) {
            // Skip non-numeric columns for summary
        }
    }

    return wine_data;
}

// Function to preprocess the data before training
df::Dataframe preprocess_data(const df::Dataframe &data) {
    std::cout << "\nPreprocessing the data..." << std::endl;

    // Create a copy of the original data
    df::Dataframe processed_data;

    // Process each column
    for (const auto &col_name : data.names()) {
        try {
            auto column = data.get<double>(col_name);

            // Normalize numeric features (except the target variable 'quality')
            if (col_name != "quality") {
                // Z-score normalization
                auto mean = df::stats::mean(column);
                auto std_dev = df::stats::std_dev(column);

                if (std_dev > 0) {
                    auto normalized =
                        column.map([mean, std_dev](double value, size_t) {
                            return (value - mean) / std_dev;
                        });

                    processed_data.add(col_name, normalized);
                    std::cout << "  - Normalized column '" << col_name << "'"
                              << std::endl;
                } else {
                    // If std_dev is 0, just center the data
                    auto centered = column.map(
                        [mean](double value, size_t) { return value - mean; });

                    processed_data.add(col_name, centered);
                    std::cout << "  - Centered column '" << col_name
                              << "' (std_dev was 0)" << std::endl;
                }
            } else {
                // Keep the target variable as is
                processed_data.add(col_name, column);
                std::cout << "  - Kept target column '" << col_name
                          << "' unchanged" << std::endl;

                // Generate a histogram of quality values
                std::map<int, int> quality_counts;
                column.forEach([&quality_counts](double value, size_t) {
                    int quality = static_cast<int>(value);
                    quality_counts[quality]++;
                });

                std::cout << "\nQuality distribution:" << std::endl;
                std::cout << "--------------------" << std::endl;

                for (const auto &[quality, count] : quality_counts) {
                    std::cout << "  Quality " << quality << ": " << count
                              << " wines";

                    // Add a simple ASCII bar
                    std::cout << " | ";
                    int bar_length = count / 10; // Scale the bar
                    for (int i = 0; i < bar_length; i++) {
                        std::cout << "#";
                    }
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception &) {
            // If column is not numeric, try to convert it or skip
            try {
                auto string_column = data.get<std::string>(col_name);

                // For this example, we'll just skip non-numeric columns
                std::cout << "  - Skipped non-numeric column '" << col_name
                          << "'" << std::endl;
            } catch (const std::exception &e) {
                std::cout << "  - Error processing column '" << col_name
                          << "': " << e.what() << std::endl;
            }
        }
    }

    return processed_data;
}

// Function to visualize feature importance
void visualize_feature_importance(const df::Dataframe &importance_df) {
    std::cout << "\nFeature Importance:" << std::endl;
    std::cout << "------------------" << std::endl;

    // Sort the features by importance
    std::vector<std::pair<std::string, double>> importance_pairs;

    auto feature_names = importance_df.get<std::string>("feature").data();
    auto importance_values = importance_df.get<double>("importance").data();

    for (size_t i = 0; i < feature_names.size() && i < importance_values.size();
         i++) {
        importance_pairs.push_back({feature_names[i], importance_values[i]});
    }

    // Sort in descending order
    std::sort(importance_pairs.begin(), importance_pairs.end(),
              [](const auto &a, const auto &b) { return a.second > b.second; });

    // Find max importance for scaling the bars
    double max_importance = 0.0;
    if (!importance_pairs.empty()) {
        max_importance = importance_pairs[0].second;
    }

    // Print importance with ASCII bar chart
    const int bar_width = 50; // Width of the ASCII bar

    for (const auto &[feature, importance] : importance_pairs) {
        int bar_length =
            static_cast<int>(importance / max_importance * bar_width);

        std::cout << std::left << std::setw(20) << feature << " | "
                  << std::fixed << std::setprecision(4) << std::setw(8)
                  << importance << " | ";

        for (int i = 0; i < bar_length; i++) {
            std::cout << "█";
        }
        std::cout << std::endl;
    }
}

// Function to visualize regression predictions vs actual values
void visualize_predictions(const df::Serie<double> &actual,
                           const df::Serie<double> &predicted) {
    std::cout << "\nPrediction Analysis:" << std::endl;
    std::cout << "-------------------" << std::endl;

    // Group predictions by actual quality
    std::map<int, std::vector<double>> predictions_by_actual;

    for (size_t i = 0; i < actual.size() && i < predicted.size(); i++) {
        int actual_quality = static_cast<int>(actual[i]);
        predictions_by_actual[actual_quality].push_back(predicted[i]);
    }

    // Print distribution of predictions for each actual quality
    for (const auto &[quality, preds] : predictions_by_actual) {
        // Calculate mean and std of predictions
        double sum = std::accumulate(preds.begin(), preds.end(), 0.0);
        double mean = sum / preds.size();

        double sq_sum = std::accumulate(preds.begin(), preds.end(), 0.0,
                                        [mean](double acc, double val) {
                                            double diff = val - mean;
                                            return acc + diff * diff;
                                        });
        double std_dev = std::sqrt(sq_sum / preds.size());

        std::cout << "Actual quality " << quality << ": ";
        std::cout << "Mean prediction = " << std::fixed << std::setprecision(2)
                  << mean;
        std::cout << ", Std = " << std::fixed << std::setprecision(2)
                  << std_dev;
        std::cout << " (from " << preds.size() << " samples)" << std::endl;

        // Simple histogram of predictions
        std::map<int, int> pred_counts;
        for (double pred : preds) {
            int rounded_pred = static_cast<int>(std::round(pred));
            pred_counts[rounded_pred]++;
        }

        // Print histogram
        std::cout << "  Histogram: ";
        for (const auto &[pred_quality, count] : pred_counts) {
            std::cout << pred_quality << "(" << count << ") ";
        }
        std::cout << std::endl;
    }
}

// Main function
int main(int argc, char **argv) {
    std::string filename = "winequality-red.csv";

    // Allow user to specify a different filename
    if (argc > 1) {
        filename = argv[1];
    }

    try {
        // Load and preprocess the data
        df::Dataframe wine_data = load_wine_data(filename);
        df::Dataframe processed_data = preprocess_data(wine_data);

        // Set the target column (what we're trying to predict)
        std::string target_column = "quality";

        // Split the data into training (80%) and testing (20%) sets
        std::cout << "\nSplitting data into training and testing sets..."
                  << std::endl;

        auto splits = df::split(5, processed_data);

        // First 4 parts (80%) for training
        df::Dataframe train_data = splits[0];
        for (size_t i = 1; i < 4; i++) {
            // Combine the dataframes (simplified for example - assumes same
            // columns)
            for (const auto &name : train_data.names()) {
                auto train_serie = train_data.get<double>(name);
                auto split_serie = splits[i].get<double>(name);

                // Concatenate the series
                std::vector<double> combined(train_serie.data());
                const auto &split_data = split_serie.data();
                combined.insert(combined.end(), split_data.begin(),
                                split_data.end());

                train_data.remove(name);
                train_data.add(name, df::Serie<double>(combined));
            }
        }

        // Last part (20%) for testing
        df::Dataframe test_data = splits[4];

        std::cout << "Training set size: "
                  << train_data.get<double>(target_column).size() << " samples"
                  << std::endl;
        std::cout << "Testing set size: "
                  << test_data.get<double>(target_column).size() << " samples"
                  << std::endl;

        // Start timing the training process
        auto start_time = std::chrono::high_resolution_clock::now();

        // Create a Random Forest regressor for predicting wine quality
        std::cout << "\nTraining a Random Forest model..." << std::endl;
        size_t num_trees = 100;
        ml::RandomForest rf = ml::create_random_forest_regressor(
            num_trees, // number of trees
            0,         // max_features (auto = sqrt(n_features))
            20,        // max_depth
            5          // min_samples_split
        );

        // Train the model
        rf.fit(train_data, target_column);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);
        std::cout << "Training completed in " << duration.count() / 1000.0
                  << " seconds." << std::endl;

        // Make predictions on the test set
        std::cout << "\nMaking predictions on the test set..." << std::endl;
        df::Serie<double> predictions = rf.predict(test_data);

        // Calculate feature importance
        std::cout << "\nCalculating feature importance..." << std::endl;
        df::Dataframe importance =
            rf.feature_importance_df(train_data, target_column);

        // Evaluate the model
        std::cout << "\nEvaluating the model..." << std::endl;
        auto metrics = rf.evaluate(test_data, target_column);

        std::cout << "Model Performance Metrics:" << std::endl;
        std::cout << "-------------------------" << std::endl;
        std::cout << "Mean Squared Error (MSE): " << std::fixed
                  << std::setprecision(4) << metrics["mse"] << std::endl;
        std::cout << "Root Mean Squared Error (RMSE): " << std::fixed
                  << std::setprecision(4) << metrics["rmse"] << std::endl;
        std::cout << "Mean Absolute Error (MAE): " << std::fixed
                  << std::setprecision(4) << metrics["mae"] << std::endl;
        std::cout << "R-squared (R2): " << std::fixed << std::setprecision(4)
                  << metrics["r2"] << std::endl;

        // Calculate out-of-bag error
        double oob_error = rf.oob_error(train_data, target_column);
        std::cout << "Out-of-Bag Error: " << std::fixed << std::setprecision(4)
                  << oob_error << std::endl;

        // Visualize feature importance
        visualize_feature_importance(importance);

        // Visualize predictions vs actual
        visualize_predictions(test_data.get<double>(target_column),
                              predictions);

        // Optional: Run a second experiment with permutation importance
        std::cout
            << "\nCalculating permutation importance (this may take longer)..."
            << std::endl;
        df::Serie<double> perm_importance =
            rf.permutation_importance(train_data, target_column, 5);

        // Create a dataframe to visualize permutation importance
        df::Dataframe perm_importance_df;
        auto feature_names = rf.get_feature_names(train_data, target_column);
        perm_importance_df.add("feature",
                               df::Serie<std::string>(feature_names));
        perm_importance_df.add("importance", perm_importance);

        std::cout << "\nPermutation Feature Importance:" << std::endl;
        std::cout << "-------------------------------" << std::endl;
        visualize_feature_importance(perm_importance_df);

        std::cout << "\nConclusion:" << std::endl;
        std::cout << "-----------" << std::endl;
        std::cout << "The Random Forest model was able to predict wine quality "
                     "with RMSE = "
                  << std::fixed << std::setprecision(4) << metrics["rmse"]
                  << std::endl;

        // Find the most important features
        std::vector<std::pair<std::string, double>> importance_pairs;
        auto imp_feature_names = importance.get<std::string>("feature").data();
        auto imp_values = importance.get<double>("importance").data();

        for (size_t i = 0;
             i < imp_feature_names.size() && i < imp_values.size(); i++) {
            importance_pairs.push_back({imp_feature_names[i], imp_values[i]});
        }

        std::sort(
            importance_pairs.begin(), importance_pairs.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });

        std::cout
            << "The most important factors for wine quality prediction are:"
            << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), importance_pairs.size());
             i++) {
            std::cout << "  " << i + 1 << ". " << importance_pairs[i].first
                      << std::endl;
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
