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

#include "../../TEST.h"
#include <array>
#include <dataframe/math/negate.h>

using namespace df;

TEST(Serie, NegateArithmetic) {
    MSG("Testing negate with arithmetic types");

    // Test with integers
    Serie<int> s1{1, -2, 3, -4, 5};
    auto result_int1 = negate(s1);
    EXPECT_ARRAY_EQ(result_int1.asArray(),
                    std::vector<int>({-1, 2, -3, 4, -5}));

    auto result_int2 = -s1;
    EXPECT_ARRAY_EQ(result_int2.asArray(),
                    std::vector<int>({-1, 2, -3, 4, -5}));

    // Test with doubles
    Serie<double> s2{1.5, -2.5, 3.5};
    auto result_double1 = negate(s2);
    EXPECT_ARRAY_NEAR(result_double1.asArray(),
                      std::vector<double>({-1.5, 2.5, -3.5}), 1e-10);

    auto result_double2 = -s2;
    EXPECT_ARRAY_NEAR(result_double2.asArray(),
                      std::vector<double>({-1.5, 2.5, -3.5}), 1e-10);

    // Test pipeline operation
    auto result_pipe = s1 | bind_negate();
    EXPECT_ARRAY_EQ(result_pipe.asArray(),
                    std::vector<int>({-1, 2, -3, 4, -5}));
}

TEST(Serie, NegateVector) {
    MSG("Testing negate with Vector3D");

    using Vector3D = std::array<double, 3>;

    Serie<Vector3D> s1{Vector3D{1.0, -2.0, 3.0}, Vector3D{-4.0, 5.0, -6.0}};

    auto result1 = negate(s1);
    auto result2 = -s1;

    // Expected results
    std::vector<Vector3D> expected{Vector3D{-1.0, 2.0, -3.0},
                                   Vector3D{4.0, -5.0, 6.0}};

    // Compare results
    EXPECT_EQ(result1.size(), expected.size());
    EXPECT_EQ(result2.size(), expected.size());
    for (size_t i = 0; i < result1.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_NEAR(result1[i][j], expected[i][j], 1e-10);
            EXPECT_NEAR(result2[i][j], expected[i][j], 1e-10);
        }
    }

    // Test pipeline operation
    auto result_pipe = s1 | bind_negate();
    EXPECT_EQ(result_pipe.size(), expected.size());
    for (size_t i = 0; i < result_pipe.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_NEAR(result_pipe[i][j], expected[i][j], 1e-10);
        }
    }
}

RUN_TESTS()