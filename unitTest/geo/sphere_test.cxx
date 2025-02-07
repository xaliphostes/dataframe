#include "../TEST.h"
#include <dataframe/geo/gen_sphere.h>
#include <dataframe/geo/mesh.h>

using namespace df;

TEST(generate_sphere, basic_properties) {
    auto sphere = generateSphere(1.0, 32, 16);
    const auto &vertices = sphere.vertices();
    const auto &triangles = sphere.triangles();

    df::print(vertices);
    df::print(triangles);

    // Test vertex count
    EXPECT_EQ(vertices.size(), ((32 + 1) * (16 + 1)));

    // Test triangle count
    EXPECT_EQ(triangles.size(), (2 * 32 * 16));

    // Test radius
    for (const auto &v : vertices.data()) {
        double r = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        EXPECT_NEAR(r, 1.0, 1e-10);
    }

    // Test vertex distribution
    bool hasTop = false;
    bool hasBottom = false;
    for (const auto &v : vertices.data()) {
        if (std::abs(v[2] - 1.0) < 1e-10)
            hasTop = true;
        if (std::abs(v[2] + 1.0) < 1e-10)
            hasBottom = true;
    }
    EXPECT_TRUE(hasTop);
    EXPECT_TRUE(hasBottom);
}

RUN_TESTS()