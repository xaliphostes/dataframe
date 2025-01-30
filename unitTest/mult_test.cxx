#include "TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/math/mult.h>
#include <limits>

TEST(Multiply, same_type_int) {
    df::Serie<int> serie1({1, 2, 3});
    df::Serie<int> serie2({2, 3, 4});

    auto result = df::mult(serie1, serie2);

    std::vector<int> expected = {2, 6, 12};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Multiply, same_type_double) {
    df::Serie<double> serie1({1.5, 2.5, 3.5});
    df::Serie<double> serie2({2.0, 3.0, 4.0});

    auto result = df::mult(serie1, serie2);

    std::vector<double> expected = {3.0, 7.5, 14.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Multiply, different_types) {
    df::Serie<int> serie1({1, 2, 3});
    df::Serie<double> serie2({2.5, 3.5, 4.5});

    auto result = df::mult(serie1, serie2);

    // Result should be double due to type promotion
    std::vector<double> expected = {2.5, 7.0, 13.5};
    EXPECT_ARRAY_EQ(result.asArray(), expected);

    // Test commutative property
    auto result2 = df::mult(serie2, serie1);
    EXPECT_ARRAY_EQ(result2.asArray(), expected);
}

TEST(Multiply, pipeline_operator) {
    df::Serie<int> serie1({1, 2, 3});
    df::Serie<double> serie2({2.5, 3.5, 4.5});
    auto result = serie1 | df::bind_mult(serie2);

    std::vector<double> expected = {2.5, 7.0, 13.5};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Multiply, empty_series) {
    df::Serie<double> empty1;
    df::Serie<double> empty2;

    auto result = df::mult(empty1, empty2);
    EXPECT_TRUE(result.empty());
}

TEST(Multiply, size_mismatch) {
    df::Serie<int> serie1({1, 2, 3});
    df::Serie<int> serie2({1, 2});

    EXPECT_THROW(df::mult(serie1, serie2), std::runtime_error);
}

TEST(Multiply, special_values) {
    df::Serie<double> serie1({std::numeric_limits<double>::infinity(),
                              -std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::quiet_NaN(),
                              std::numeric_limits<double>::min(),
                              std::numeric_limits<double>::max(), 0.0});

    df::Serie<double> serie2(
        {2.0, 2.0, 1.0, 2.0, 2.0, std::numeric_limits<double>::infinity()});

    auto result = df::mult(serie1, serie2);

    std::vector<double> expected = {std::numeric_limits<double>::infinity(),
                                    -std::numeric_limits<double>::infinity(),
                                    std::numeric_limits<double>::quiet_NaN(),
                                    2.0 * std::numeric_limits<double>::min(),
                                    std::numeric_limits<double>::infinity(),
                                    std::numeric_limits<double>::quiet_NaN()};

    for (size_t i = 0; i < result.size(); ++i) {
        if (std::isnan(expected[i])) {
            EXPECT_TRUE(std::isnan(result[i]));
        } else {
            EXPECT_EQ(result[i], expected[i]);
        }
    }
}

TEST(Multiply, type_promotion) {
    // int * int -> int
    df::Serie<int> i1({1, 2}), i2({3, 4});
    auto r1 = df::mult(i1, i2);
    static_assert(std::is_same<decltype(r1)::value_type, int>::value,
                  "int * int should give int");

    // int * double -> double
    df::Serie<double> d1({1.0, 2.0});
    auto r2 = df::mult(i1, d1);
    static_assert(std::is_same<decltype(r2)::value_type, double>::value,
                  "int * double should give double");

    // float * double -> double
    df::Serie<float> f1({1.0f, 2.0f});
    auto r3 = df::mult(f1, d1);
    static_assert(std::is_same<decltype(r3)::value_type, double>::value,
                  "float * double should give double");
}

RUN_TESTS();