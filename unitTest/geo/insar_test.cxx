#include "../TEST.h"
#include <dataframe/geo/insar.h>

using namespace df;

TEST(insar, basic_operations) {
    Serie<Vector3> displacements = {
        {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

    Vector3 los = {1.0, 0.0, 0.0};
    auto insar_vals = insar(displacements, los);

    EXPECT_EQ(insar_vals.size(), 3);
    EXPECT_NEAR(insar_vals[0], 1.0, 1e-10);
    EXPECT_NEAR(insar_vals[1], 0.0, 1e-10);
    EXPECT_NEAR(insar_vals[2], 0.0, 1e-10);

    auto fringe_vals = fringes(insar_vals, 0.5);
    EXPECT_EQ(fringe_vals.size(), 3);
}

RUN_TESTS()