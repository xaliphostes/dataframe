/**
 * RandomForest.h
 * A C++ implementation of Random Forest algorithm integrated with the DataFrame
 * library
 */

#pragma once
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>

namespace ml {

class DecisionTree;

enum class TaskType { REGRESSION, CLASSIFICATION };

/**
 * Example of using the RandomForest implementation with the DataFrame library:
 * @code
 * #include <dataframe/Serie.h>
 * #include <dataframe/Dataframe.h>
 * #include <dataframe/io/csv.h>
 * #include <dataframe/core/split.h>
 * #include <dataframe/ml/random_forest.h>
 * #include <iostream>
 *
 * int main() {
 *     // Load data from CSV
 *     df::Dataframe data = df::io::read_csv("iris.csv");
 *
 *     // Split data into training and testing sets (80/20)
 *     auto splits = df::split(5, data);
 *
 *     // Combine first 4 parts for training (80%)
 *     df::Dataframe train_data;
 *     for (size_t i = 0; i < 4; ++i) {
 *         // Assuming each split is a tuple with a single Dataframe
 *         const auto& split_data = splits[i];
 *         // Merge into train_data...
 *     }
 *
 *     // Use the 5th part for testing (20%)
 *     df::Dataframe test_data = splits[4];
 *
 *     // Create and train a random forest classifier
 *     ml::RandomForest rf = ml::create_random_forest_classifier(
 *         100,    // num_trees
 *         3,      // n_classes
 *         0,      // max_features (auto)
 *         10,     // max_depth
 *         2       // min_samples_split
 *     );
 *
 *     // Train the model
 *     rf.fit(train_data, "species");
 *
 *     // Make predictions
 *     df::Serie<double> predictions = rf.predict(test_data);
 *
 *     // Calculate feature importance
 *     df::Dataframe importance = rf.feature_importance_df(train_data, "species");
 *
 *     // Print feature importance
 *     importance.forEach([](const std::string& feature, double imp) {
 *         std::cout << feature << ": " << imp << std::endl;
 *     });
 *
 *     // Evaluate the model
 *     auto metrics = rf.evaluate(test_data, "species");
 *     std::cout << "Accuracy: " << metrics["accuracy"] << std::endl;
 *
 *     return 0;
 * }
 * @endcode
 */
class RandomForest {
  public:
    // Constructor
    RandomForest(size_t num_trees = 100,
                 TaskType task_type = TaskType::REGRESSION,
                 size_t max_features = 0,
                 size_t max_depth = std::numeric_limits<size_t>::max(),
                 size_t min_samples_split = 2, size_t n_classes = 0);

    void fit(const df::Dataframe &data, const std::string &target_column);

    df::Serie<double> predict(const df::Dataframe &data);

    df::Serie<double> feature_importance(const df::Dataframe &data,
                                         const std::string &target_column);

    double oob_error(const df::Dataframe &data,
                     const std::string &target_column);

    df::Serie<double> permutation_importance(const df::Dataframe &data,
                                             const std::string &target_column,
                                             size_t n_repeats = 10);

    std::vector<std::string>
    get_feature_names(const df::Dataframe &data,
                      const std::string &target_column) const;

    df::Dataframe feature_importance_df(const df::Dataframe &data,
                                        const std::string &target_column);

    size_t get_num_trees() const { return num_trees; }

    TaskType get_task_type() const { return task_type; }

    std::map<std::string, double> evaluate(const df::Dataframe &data,
                                           const std::string &target_column);

  private:
    std::vector<DecisionTree> trees;
    TaskType task_type;
    size_t num_trees;
    size_t max_features;
    size_t max_depth;
    size_t min_samples_split;
    size_t n_classes; // For classification

    std::tuple<std::vector<std::vector<double>>, std::vector<double>>
    bootstrapSample(const std::vector<std::vector<double>> &,
                    const std::vector<double> &) const;

    std::tuple<std::vector<std::vector<double>>, std::vector<double>>
    extractFeatures(const df::Dataframe &, const std::string &) const;

    std::vector<std::vector<double>>
    extractFeaturesForPrediction(const df::Dataframe &) const;

    template <typename T>
    std::vector<double> convertToDoubleVector(const std::vector<T> &) const;

    // Compute out-of-bag (OOB) samples for a bootstrap sample
    std::vector<size_t> computeOOBSamples(const std::vector<size_t> &,
                                          size_t) const;
};

// Utility function to create a Random Forest Regressor
RandomForest create_random_forest_regressor(
    size_t num_trees = 100, size_t max_features = 0,
    size_t max_depth = std::numeric_limits<size_t>::max(),
    size_t min_samples_split = 2);

// Utility function to create a Random Forest Classifier
RandomForest create_random_forest_classifier(
    size_t num_trees = 100, size_t n_classes = 0, size_t max_features = 0,
    size_t max_depth = std::numeric_limits<size_t>::max(),
    size_t min_samples_split = 2);

} // namespace ml

#include "inline/random_forest.hxx"