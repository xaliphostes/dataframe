#include "TEST.h"
#include <dataframe/map.h>
#include <dataframe/math/random.h>
#include <dataframe/utils/filter.h>
#include <set>

TEST(Random, basic_int) {
    const size_t n = 1000;
    const int min = -10;
    const int max = 10;

    auto serie = df::random<int>(n, min, max);

    // Check size
    EXPECT_EQ(serie.size(), n);

    // Check bounds
    bool in_bounds = true;
    serie.forEach([&](int value, size_t) {
        if (value < min || value > max) {
            in_bounds = false;
        }
    });
    EXPECT_TRUE(in_bounds);

    // Check distribution (should have reasonable spread)
    std::set<int> unique_values;
    serie.forEach([&](int value, size_t) { unique_values.insert(value); });

    // With 1000 samples between -10 and 10, we should have most values
    // represented
    EXPECT_GT(unique_values.size(), 15); // At least 75% of possible values
}

TEST(Random, basic_double) {
    const size_t n = 1000;
    const double min = 0.0;
    const double max = 1.0;

    auto serie = df::random<double>(n, min, max);

    // Check size
    EXPECT_EQ(serie.size(), n);

    // Check bounds
    bool in_bounds = true;
    serie.forEach([&](double value, size_t) {
        if (value < min || value > max) {
            in_bounds = false;
        }
    });
    EXPECT_TRUE(in_bounds);

    // Check that we have good spread of values
    double sum = 0.0;
    serie.forEach([&](double value, size_t) { sum += value; });

    // Mean should be approximately 0.5
    double mean = sum / n;
    EXPECT_NEAR(mean, 0.5, 0.1);
}

TEST(Random, generic_type) {
    // Test with float
    auto float_serie = df::random<float>(100, -0.0f, 1.0f);
    EXPECT_EQ(float_serie.size(), 100);

    // Test with long
    auto long_serie = df::random<long>(50, -100L, 100L);
    EXPECT_EQ(long_serie.size(), 50);
}

TEST(Random, empty) {
    auto serie = df::random<int>(0, 0, 10);
    EXPECT_TRUE(serie.empty());
}

TEST(Random, single_value) {
    auto serie = df::random<int>(1, 42, 42);
    EXPECT_EQ(serie.size(), 1);
    EXPECT_EQ(serie[0], 42);
}

TEST(Random, pipe_example) {
    using namespace df;

    // Create random series and transform them
    auto result = df::random<double>(100, 0.0, 1.0) |
                  df::bind_map([](double v, size_t) { return v * 2.0; }) |
                  df::bind_filter([](double v, size_t) { return v > 1.0; });

    EXPECT_GT(result.size(), 0);
    EXPECT_LT(result.size(), 100);
}

RUN_TESTS();