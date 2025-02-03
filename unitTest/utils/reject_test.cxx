#include "../TEST.h"
#include <dataframe/pipe.h>
#include <dataframe/utils/reject.h>

using namespace df;

TEST(reject, basic) {
    MSG("Testing basic reject functionality");

    // Reject even numbers
    Serie<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto result = reject([](int x, size_t) { return x % 2 == 0; }, numbers);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<int>({1, 3, 5, 7, 9}));

    // Reject negative numbers
    Serie<double> mixed{-2.0, -1.0, 0.0, 1.0, 2.0};
    auto positives = reject([](double x, size_t) { return x < 0; }, mixed);
    EXPECT_ARRAY_EQ(positives.asArray(), std::vector<double>({0.0, 1.0, 2.0}));
}

TEST(reject, predicates) {
    MSG("Testing predefined predicates");

    Serie<double> values{-2.0, -1.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0};

    // Test less_than
    auto result1 = reject(less_than(0.0), values);
    EXPECT_ARRAY_EQ(result1.asArray(),
                    std::vector<double>({0.0, 1.0, 2.0, 3.0, 4.0, 5.0}));

    // Test greater_than
    auto result2 = reject(greater_than(3.0), values);
    EXPECT_ARRAY_EQ(result2.asArray(),
                    std::vector<double>({-2.0, -1.0, 0.0, 1.0, 2.0, 3.0}));

    // Test outside
    auto result3 = reject(outside(0.0, 3.0), values);
    EXPECT_ARRAY_EQ(result3.asArray(),
                    std::vector<double>({0.0, 1.0, 2.0, 3.0}));

    // Test equal_to
    auto result4 = reject(equal_to(0.0), values);
    EXPECT_ARRAY_EQ(result4.asArray(),
                    std::vector<double>({-2.0, -1.0, 1.0, 2.0, 3.0, 4.0, 5.0}));
}

TEST(reject, multi_series) {
    MSG("Testing multi-series reject");

    Serie<double> s1{1.0, 2.0, 3.0, 4.0, 5.0};
    Serie<double> s2{5.0, 4.0, 3.0, 2.0, 1.0};

    // Reject where s1 > s2
    auto result =
        reject([](double x1, double x2, size_t) { return x1 > x2; }, s1, s2);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<double>({1.0, 2.0, 3.0}));

    // Test error on size mismatch
    Serie<double> s3{1.0, 2.0};
    EXPECT_THROW(
        reject([](double x1, double x2, size_t) { return x1 > x2; }, s1, s3),
        std::runtime_error);
}

TEST(reject, vectors) {
    MSG("Testing reject with vector types");

    Serie<Vector2> vectors{{1.0, 1.0}, {2.0, 0.0}, {0.0, 2.0}, {3.0, 3.0}};

    // Reject vectors with any component > 2.0
    auto result = reject(
        [](const Vector2 &v, size_t) { return v[0] > 2.0 || v[1] > 2.0; },
        vectors);

    std::vector<Vector2> expected{{1.0, 1.0}, {2.0, 0.0}, {0.0, 2.0}};
    EXPECT_EQ(result.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_ARRAY_NEAR(result[i], expected[i], 1e-10);
    }
}

TEST(reject, pipeline) {
    MSG("Testing pipeline operations");

    Serie<double> values{-2.0, -1.0, 0.0, 1.0, 2.0};

    // Test pipeline with lambda
    auto result1 = values | bind_reject([](double x, size_t) { return x < 0; });
    EXPECT_ARRAY_EQ(result1.asArray(), std::vector<double>({0.0, 1.0, 2.0}));

    // Test pipeline with predefined predicate
    auto result2 = values | bind_reject(less_than(0.0));
    EXPECT_ARRAY_EQ(result2.asArray(), std::vector<double>({0.0, 1.0, 2.0}));

    // Chain multiple rejections
    auto result3 =
        values | bind_reject(less_than(-1.0)) | bind_reject(greater_than(1.0));
    EXPECT_ARRAY_EQ(result3.asArray(), std::vector<double>({-1.0, 0.0, 1.0}));
}

RUN_TESTS()
