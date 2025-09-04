#include "../../TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/stats/kriging.h>

using namespace df;

// Helper function to create a simple test dataset
std::pair<Serie<Vector2>, Serie<double>> create_test_dataset() {
    Serie<Vector2> positions = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0},
                                {1.0, 1.0}, {0.5, 0.5}, {0.0, 0.5},
                                {1.0, 0.5}, {0.5, 0.0}, {0.5, 1.0}};

    // Create a smooth surface z = x^2 + y^2
    Serie<double> values(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        const auto &pos = positions[i];
        values[i] = pos[0] * pos[0] + pos[1] * pos[1];
    }

    return {positions, values};
}

// Test experimental variogram calculation
TEST(ExperimentalVariogram, Basic) {
    MSG("Testing experimental variogram calculation");

    auto [positions, values] = create_test_dataset();
    double lag_distance = 0.2;
    size_t n_lags = 10;

    auto [distances, variogram] = df::stats::calculate_experimental_variogram(
        positions, values, lag_distance, n_lags);

    // Check output sizes
    EXPECT_EQ(distances.size(), n_lags);
    EXPECT_EQ(variogram.size(), n_lags);

    // Check that variogram values are non-negative
    for (size_t i = 0; i < variogram.size(); ++i) {
        EXPECT_GE(variogram[i], 0.0);
    }

    // Check that distances are monotonically increasing
    for (size_t i = 1; i < distances.size(); ++i) {
        if (distances[i] > 0) { // Skip empty bins
            EXPECT_GT(distances[i], distances[i - 1]);
        }
    }
}

// Test theoretical variogram models
TEST(VariogramModel, Models) {
    MSG("Testing theoretical variogram models");

    df::stats::VariogramParams params;
    params.nugget = 0.1;
    params.sill = 1.0;
    params.range = 2.0;

    // Test Spherical model
    params.model = df::stats::VariogramModel::Spherical;
    EXPECT_NEAR(variogram_model(0.0, params), 0.0, 1e-10);
    EXPECT_NEAR(variogram_model(params.range, params),
                params.nugget + params.sill, 1e-10);
    EXPECT_NEAR(variogram_model(params.range * 2, params),
                params.nugget + params.sill, 1e-10);

    // Test Exponential model
    params.model = df::stats::VariogramModel::Exponential;
    EXPECT_NEAR(variogram_model(0.0, params), 0.0, 1e-10);
    EXPECT_GT(variogram_model(params.range, params),
              0.95 * (params.nugget + params.sill));

    // Test Gaussian model
    params.model = df::stats::VariogramModel::Gaussian;
    EXPECT_NEAR(variogram_model(0.0, params), 0.0, 1e-10);
    EXPECT_GT(variogram_model(params.range, params),
              0.95 * (params.nugget + params.sill));
}

// Test ordinary kriging
TEST(OrdinaryKriging, Interpolation) {
    MSG("Testing ordinary kriging interpolation");

    auto [known_positions, known_values] = create_test_dataset();

    // Create query points
    Serie<Vector2> query_positions = {{0.25, 0.25}, {0.75, 0.75}, {0.5, 0.5}};

    df::stats::VariogramParams params;
    params.nugget = 0.001;
    params.sill = 1.0;
    params.range = 1.0;
    params.model = df::stats::VariogramModel::Spherical;

    auto [estimates, variances] = ordinary_kriging(
        known_positions, known_values, query_positions, params);

    // Check output sizes
    EXPECT_EQ(estimates.size(), query_positions.size());
    EXPECT_EQ(variances.size(), query_positions.size());

    // Check that variances are non-negative
    for (const auto &var : variances.data()) {
        EXPECT_GE(var, 0.0);
    }

    // Test interpolation at known point (0.5, 0.5)
    // Should be close to actual value
    double actual_value = 0.5 * 0.5 + 0.5 * 0.5; // x^2 + y^2
    EXPECT_NEAR(estimates[2], actual_value, 0.1);

    // Test that kriging variance is smaller at known points
    EXPECT_NEAR(variances[2], params.nugget, 0.1);
}

// Test kriging cross-validation
TEST(OrdinaryKriging, CrossValidation) {
    MSG("Testing kriging cross-validation");

    auto [positions, values] = create_test_dataset();

    df::stats::VariogramParams params;
    params.nugget = 0.001;
    params.sill = 1.0;
    params.range = 1.0;
    params.model = df::stats::VariogramModel::Spherical;

    // Perform leave-one-out cross-validation
    std::vector<double> errors;
    for (size_t i = 0; i < positions.size(); ++i) {
        // Create datasets excluding point i
        Serie<Vector2> train_positions;
        Serie<double> train_values;
        for (size_t j = 0; j < positions.size(); ++j) {
            if (j != i) {
                train_positions.add(positions[j]);
                train_values.add(values[j]);
            }
        }

        // Predict at left-out point
        Serie<Vector2> test_position = {positions[i]};
        auto [estimate, variance] = ordinary_kriging(
            train_positions, train_values, test_position, params);

        errors.push_back(std::abs(estimate[0] - values[i]));
    }

    // Calculate mean absolute error
    double mae =
        std::accumulate(errors.begin(), errors.end(), 0.0) / errors.size();

    // Check that cross-validation error is reasonable
    EXPECT_LT(mae, 0.2); // Adjust threshold based on your data
}

RUN_TESTS()