# LIME: Local Interpretable Model-agnostic Explanations

## Overview

LIME (Local Interpretable Model-agnostic Explanations) is a technique to explain the predictions of any machine learning model by approximating it locally with an interpretable model. This document describes the LIME implementation for the DataFrame library, including its design, functionality, and usage.

## Core Concept

LIME works on the principle that while a model may be globally complex, its behavior can be approximated locally with a simpler model. For a specific prediction, LIME:

1. Generates perturbed samples around the instance being explained
2. Gets predictions from the original "black box" model for these samples
3. Weights samples based on their proximity to the original instance
4. Fits a simple, interpretable model (like linear regression) to this local dataset
5. Extracts feature weights from this interpretable model as the explanation

## Implementation Details

### Key Components

- **Lime Class**: Main implementation that handles the explanation process
- **SimpleLinearRegression**: Helper class that provides weighted linear regression functionality
- **Support for Categorical Features**: Specialized handling for both numerical and categorical data

### Algorithm Flow

1. **Perturbation Generation**:
   - For numerical features: Add Gaussian noise centered at the original value
   - For categorical features: Randomly sample from the set of possible values
   - Result: A DataFrame of perturbed samples that are variations of the original instance

2. **Distance & Weight Calculation**:
   - Compute the distance between each perturbed sample and the original instance
   - For numerical features: Normalized squared difference
   - For categorical features: Simple 0/1 distance (same/different)
   - Apply an exponential kernel to calculate weights: `weight = exp(-(distance^2)/(kernel_width^2))`

3. **Black Box Predictions**:
   - Get predictions from the original model for all perturbed samples
   - These predictions represent the behavior of the complex model in the local region

4. **Feature Transformation**:
   - Convert categorical features into binary features (one-hot encoding minus one category)
   - Numerical features remain as-is
   - Result: A feature matrix suitable for linear modeling

5. **Interpretable Model Fitting**:
   - Fit a weighted linear regression model to the perturbed samples
   - Use sample weights based on proximity to the original instance
   - Target variable: Predictions from the black box model

6. **Explanation Extraction**:
   - Extract coefficients from the linear model
   - Rank features by importance (magnitude of coefficients)
   - Return top N features with their coefficients (positive or negative impact)

### Handling Specific Data Types

**Numerical Features**:
- Perturbed by adding Gaussian noise
- Standard deviation determined from training data distribution
- Distance calculated using normalized squared difference

**Categorical Features**:
- Converted to binary features for the linear model
- Perturbed by randomly sampling from possible values
- Distance calculated using simple match/non-match metric

## Usage Example

```cpp
// Load data and train a model
df::Dataframe data = df::io::read_csv("data.csv");
auto model = ml::create_random_forest_classifier(100, 3);
model.fit(data, "target");

// Define categorical features
std::set<std::string> categorical_features{"category1", "category2", "target"};

// Create LIME explainer
ml::Lime lime_explainer(data, "target", categorical_features);

// Choose an instance to explain
df::Dataframe instance = get_dataframe_row(data, 10);

// Create a prediction function
auto predict_fn = [&model](const df::Dataframe& samples) {
    return model.predict(samples);
};

// Generate explanation
auto explanation = lime_explainer.explain(
    instance,
    predict_fn,
    5,    // Number of features to include
    1000  // Number of samples to generate
);

// Display explanation
for (const auto& [feature, weight] : explanation) {
    std::cout << feature << ": " << weight << std::endl;
}
```

## Integration with DataFrame Library

This implementation integrates with the DataFrame library by:
- Using `Dataframe` for storing data and `Serie` for individual columns
- Leveraging existing functions for data manipulation
- Following the same design patterns as other ML components
- Providing a consistent API experience

## Benefits and Applications

### Key Benefits

- **Transparency**: Makes model predictions understandable by showing which features are most important
- **Trust**: Helps users trust complex models by providing explanations for individual predictions
- **Debugging**: Assists in identifying when models learn spurious correlations or biases
- **Compliance**: Supports regulatory requirements for model explanations in certain domains

### Practical Applications

- **Healthcare**: Explaining diagnoses or treatment recommendations
- **Finance**: Justifying credit decisions or fraud detection
- **Legal**: Supporting fair and transparent automated decision-making
- **Scientific Research**: Understanding relationships discovered by models

## Future Extensions

Potential enhancements to the LIME implementation include:

1. **Alternative Kernels**: Supporting different kernel functions for weighting samples
2. **Categorical Encodings**: More sophisticated handling of categorical features
3. **Interactive Visualizations**: Visual representations of explanations
4. **Global Explanations**: Aggregating local explanations for global model interpretation
5. **Specialized Explainers**: Adapting the algorithm for text, image, or time series data

## References

1. Ribeiro, M.T., Singh, S. and Guestrin, C. (2016). "Why Should I Trust You?": Explaining the Predictions of Any Classifier. KDD '16.
2. Molnar, C. (2019). Interpretable Machine Learning. https://christophm.github.io/interpretable-ml-book/

# `iris.csv`
File from https://raw.githubusercontent.com/mwaskom/seaborn-data/master/iris.csv


