/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "../TEST.h"
#include <dataframe/geo/gen_sphere.h>
#include <dataframe/geo/mesh/mesh.h>

using namespace df;

TEST(generate_sphere, basic_properties) {
    const auto& [vertices, triangles] = generateSphere(1.0, 32, 16);

    // df::print(vertices);
    // df::print(triangles);

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