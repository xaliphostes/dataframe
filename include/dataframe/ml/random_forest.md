# Key Components Implemented

## Feature Importance Calculation

- `feature_importance`: Calculates the importance of each feature based on the mean decrease in impurity (MDI) across all trees.
- `permutation_importance`: A more reliable method that measures how much the model's performance decreases when a feature is randomly shuffled.
- The feature importance is normalized so the values sum to 1, making them easier to interpret.


## Bootstrap Sampling

- `bootstrapSample`: Creates bootstrap samples (random sampling with replacement) for training individual trees.
- This ensures that each tree in the forest is trained on a slightly different subset of the data.


## Feature Extraction

- `extractFeatures`: Converts DataFrame columns into the feature matrix and target vector needed for model training.
- `extractFeaturesForPrediction`: Extracts features for making predictions on new data.


## Out-of-Bag (OOB) Error Estimation

- `oob_error`: Calculates the out-of-bag error, which is the prediction error on the samples not used for training each tree.
- `computeOOBSamples`: Identifies which samples weren't used in a particular bootstrap sample.


## Model Evaluation

- `evaluate`: Calculates various performance metrics:

    - For regression: MSE, RMSE, MAE, R-squared
    - For classification: Accuracy, and for binary classification, also precision, recall, and F1 score




## Helper Classes and Functions

- `DecisionNode` and `DecisionTree` classes for building the individual trees in the forest
- `create_random_forest_regressor` and - `create_random_forest_classifier` utility functions



# Implementation Details

- The implementation supports both regression and classification tasks through the `TaskType` enum.
- For classification, it handles multi-class problems and provides appropriate metrics.
- The code is optimized with OpenMP for parallel training of trees (`#pragma omp parallel for`).
- The implementation includes proper handling of feature subsets during tree construction (the "random" part of Random Forest).
- The API is designed to integrate seamlessly with the DataFrame library, making it easy to use in data analysis pipelines.

# Usage Example
The example at the end demonstrates how to use this implementation in a complete workflow:

1. Loading data from a CSV file
1. Splitting the data into training and testing sets
1. Creating and training a Random Forest classifier
1. Making predictions on the test set
1. Calculating and printing feature importance
1. Evaluating the model's performance