#include "TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/math/scale.h>

TEST(Scale, scalar_multiplication) {
    df::Serie<double> serie({1.0, 2.0, 3.0, 4.0, 5.0});
    double scalar = 2.0;

    auto result = df::scale(serie, scalar);

    std::vector<double> expected = {2.0, 4.0, 6.0, 8.0, 10.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, elementwise_multiplication) {
    df::Serie<double> serie({1.0, 2.0, 3.0, 4.0, 5.0});
    df::Serie<double> scalars({2.0, 3.0, 4.0, 5.0, 6.0});

    auto result = df::scale(serie, scalars);

    std::vector<double> expected = {2.0, 6.0, 12.0, 20.0, 30.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, empty_series) {
    df::Serie<double> serie;
    double scalar = 2.0;

    auto result = df::scale(serie, scalar);
    EXPECT_TRUE(result.empty());
}

TEST(Scale, size_mismatch) {
    df::Serie<double> serie({1.0, 2.0, 3.0});
    df::Serie<double> scalars({2.0, 3.0});

    EXPECT_THROW(df::scale(serie, scalars), std::runtime_error);
}

TEST(Scale, integer_type) {
    df::Serie<int> serie({1, 2, 3, 4, 5});
    int scalar = 3;

    auto result = df::scale(serie, scalar);

    std::vector<int> expected = {3, 6, 9, 12, 15};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, pipeline_scalar) {
    df::Serie<double> serie({1.0, 2.0, 3.0, 4.0, 5.0});

    auto result = serie | df::bind_scale<double>(2.0);

    std::vector<double> expected = {2.0, 4.0, 6.0, 8.0, 10.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, pipeline_elementwise) {
    df::Serie<double> serie({1.0, 2.0, 3.0});
    df::Serie<double> scalars({2.0, 3.0, 4.0});

    auto result = serie | df::bind_scale<double>(scalars);

    std::vector<double> expected = {2.0, 6.0, 12.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

// --------------------------------------------------------
// User defined type

// Helper function for comparing doubles, handling special values
bool almost_equal(double a, double b) {
    if (std::isnan(a) && std::isnan(b))
        return true;
    if (std::isinf(a) && std::isinf(b)) {
        // Check if both are same sign infinity
        return (a > 0) == (b > 0);
    }
    if (std::isinf(a) || std::isinf(b) || std::isnan(a) || std::isnan(b)) {
        return false;
    }

    // For very small numbers near zero
    if (std::abs(a) < std::numeric_limits<double>::min() &&
        std::abs(b) < std::numeric_limits<double>::min()) {
        return true;
    }

    const double epsilon = 1e-10;
    const double diff = std::abs(a - b);
    const double abs_a = std::abs(a);
    const double abs_b = std::abs(b);

    // For numbers very close to zero
    if (abs_a < epsilon && abs_b < epsilon) {
        return diff < epsilon;
    }

    // For regular numbers, use relative comparison
    return diff <= epsilon * std::max(abs_a, abs_b);
}

struct Vector3D {
    double x, y, z;

    Vector3D(double x = 0.0, double y = 0.0, double z = 0.0)
        : x(x), y(y), z(z) {}

    Vector3D operator*(double scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D operator*(const Vector3D &other) const {
        return Vector3D(x * other.x, y * other.y, z * other.z);
    }

    bool operator==(const Vector3D &other) const {
        return almost_equal(x, other.x) && almost_equal(y, other.y) &&
               almost_equal(z, other.z);
    }

    bool operator!=(const Vector3D &other) const { return !(*this == other); }
};

std::ostream &operator<<(std::ostream &os, const Vector3D &v) {
    os << "Vector3D(";
    if (std::isnan(v.x))
        os << "nan";
    else if (std::isinf(v.x))
        os << (v.x > 0 ? "inf" : "-inf");
    else
        os << v.x;
    os << ", ";

    if (std::isnan(v.y))
        os << "nan";
    else if (std::isinf(v.y))
        os << (v.y > 0 ? "inf" : "-inf");
    else
        os << v.y;
    os << ", ";

    if (std::isnan(v.z))
        os << "nan";
    else if (std::isinf(v.z))
        os << (v.z > 0 ? "inf" : "-inf");
    else
        os << v.z;
    os << ")";
    return os;
}

TEST(Scale, vector3d_scalar) {
    // Create a Serie of Vector3D
    df::Serie<Vector3D> serie({Vector3D(1.0, 2.0, 3.0), Vector3D(4.0, 5.0, 6.0),
                               Vector3D(-1.0, -2.0, -3.0)});

    double scalar = 2.0;
    auto result = df::scale(serie, scalar);

    std::vector<Vector3D> expected = {Vector3D(2.0, 4.0, 6.0),
                                      Vector3D(8.0, 10.0, 12.0),
                                      Vector3D(-2.0, -4.0, -6.0)};

    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, vector3d_elementwise) {
    // Create two Series of Vector3D
    df::Serie<Vector3D> serie({Vector3D(1.0, 2.0, 3.0), Vector3D(4.0, 5.0, 6.0),
                               Vector3D(-1.0, -2.0, -3.0)});

    df::Serie<Vector3D> scalars({Vector3D(2.0, 3.0, 4.0),
                                 Vector3D(0.5, 1.0, 1.5),
                                 Vector3D(-1.0, -1.0, -1.0)});

    auto result = df::scale(serie, scalars);

    std::vector<Vector3D> expected = {Vector3D(2.0, 6.0, 12.0),
                                      Vector3D(2.0, 5.0, 9.0),
                                      Vector3D(1.0, 2.0, 3.0)};

    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

TEST(Scale, vector3d_pipeline) {
    df::Serie<Vector3D> serie(
        {Vector3D(1.0, 2.0, 3.0), Vector3D(4.0, 5.0, 6.0)});

    // Test pipeline with scalar
    auto result1 = serie | df::bind_scale<double>(2.0);
    std::vector<Vector3D> expected1 = {Vector3D(2.0, 4.0, 6.0),
                                       Vector3D(8.0, 10.0, 12.0)};
    EXPECT_ARRAY_EQ(result1.asArray(), expected1);

    // Test pipeline with element-wise scaling
    df::Serie<Vector3D> scalars(
        {Vector3D(2.0, 3.0, 4.0), Vector3D(0.5, 1.0, 1.5)});

    auto result2 = serie | df::bind_scale(scalars);
    std::vector<Vector3D> expected2 = {Vector3D(2.0, 6.0, 12.0),
                                       Vector3D(2.0, 5.0, 9.0)};
    EXPECT_ARRAY_EQ(result2.asArray(), expected2);
}

// Test mixed operations with negative values and zero
TEST(Scale, vector3d_edge_cases) {
    df::Serie<Vector3D> serie(
        {Vector3D(0.0, 0.0, 0.0), Vector3D(-1.0, 2.0, -3.0),
         Vector3D(std::numeric_limits<double>::min(),
                  std::numeric_limits<double>::max(), 1.0)});

    // Test with negative scalar
    auto result = df::scale(serie, -2.0);

    std::vector<Vector3D> expected = {
        Vector3D(0.0, 0.0, 0.0), Vector3D(2.0, -4.0, 6.0),
        Vector3D(-2.0 * std::numeric_limits<double>::min(),
                 -2.0 * std::numeric_limits<double>::max(), -2.0)};

    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

RUN_TESTS();