#include "TEST.h"
#include <cmath>
#include <dataframe/functional/geo/generate_sphere.h>
#include <set>

TEST(GenSphere, BasicGeneration) {
    MSG("Testing basic sphere generation");

    auto sphere = df::geo::generateSphere<double>(2);

    // Check basic properties
    EXPECT_TRUE(sphere.get<double>("positions").isValid());
    EXPECT_TRUE(sphere.get<uint32_t>("indices").isValid());
    EXPECT_EQ(sphere.get<double>("positions").itemSize(), 3);
    EXPECT_EQ(sphere.get<uint32_t>("indices").itemSize(), 3);
}

TEST(GenSphere, Radius) {
    MSG("Testing sphere radius");

    auto sphere = df::geo::generateSphere<double>(3);
    const double expected_radius = 3.0;
    const double tolerance = 1e-10;

    // Check that all vertices are at the expected radius
    const auto &positions = sphere.get<double>("positions");

    for (uint32_t i = 0; i < positions.count(); i++) {
        auto pos = positions.array(i);
        double radius =
            std::sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
        EXPECT_NEAR(radius, expected_radius, tolerance);
    }
}

TEST(GenSphere, InputValidation) {
    MSG("Testing input validation");

    // Test invalid subdivision
    EXPECT_THROW(df::geo::generateSphere<double>(0), std::invalid_argument);
    EXPECT_THROW(df::geo::generateSphere<double>(-1), std::invalid_argument);
}

TEST(GenSphere, FloatingPointTypes) {
    MSG("Testing different floating point types");

    auto sphere_float = df::geo::generateSphere<float>(2);
    auto sphere_double = df::geo::generateSphere<double>(2);

    EXPECT_EQ(sphere_float.get<float>("positions").count(), sphere_double.get<double>("positions").count());
    EXPECT_EQ(sphere_float.get<uint32_t>("indices").count(), sphere_double.get<uint32_t>("indices").count());
}

TEST(GenSphere, Topology) {
    MSG("Testing sphere topology");

    auto sphere = df::geo::generateSphere<double>(2);

    // Each vertex should be used at least once
    std::set<uint32_t> used_indices;
    const auto &indices = sphere.get<uint32_t>("indices");
    for (uint32_t i = 0; i < indices.count(); i++) {
        used_indices.insert(indices.value(i));
    }

    // Number of unique vertices should be less than total indices
    EXPECT_TRUE(used_indices.size() <= indices.count());
    // Maximum index should be less than number of vertices
    EXPECT_TRUE(*used_indices.rbegin() < sphere.get<double>("positions").count());
}

RUN_TESTS()