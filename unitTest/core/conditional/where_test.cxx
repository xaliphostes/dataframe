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
#include <dataframe/utils/where.h>

using namespace df;

TEST(Serie, WhereArithmetic) {
    MSG("Testing where with arithmetic types");

    // Test with boolean condition
    Serie<bool> condition{true, false, true, false, true};
    Serie<int> then_serie{1, 2, 3, 4, 5};
    Serie<int> else_serie{10, 20, 30, 40, 50};

    auto result = where(condition, then_serie, else_serie);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<int>({1, 20, 3, 40, 5}));

    // Test with numeric condition
    Serie<double> num_condition{1.0, 0.0, -1.0, 0.0, 2.0};
    auto result2 = where(num_condition, then_serie, else_serie);
    EXPECT_ARRAY_EQ(result2.asArray(), std::vector<int>({1, 20, 3, 40, 5}));

    // Test with scalar then/else values
    auto result3 = where(condition, 100, -100);
    EXPECT_ARRAY_EQ(result3.asArray(),
                    std::vector<int>({100, -100, 100, -100, 100}));

    // Test pipeline operation with series
    auto result_pipe = condition | bind_where(then_serie, else_serie);
    EXPECT_ARRAY_EQ(result_pipe.asArray(), std::vector<int>({1, 20, 3, 40, 5}));

    // Test pipeline operation with scalars
    auto result_pipe2 = condition | bind_where(100, -100);
    EXPECT_ARRAY_EQ(result_pipe2.asArray(),
                    std::vector<int>({100, -100, 100, -100, 100}));
}

TEST(Serie, WhereVector) {
    MSG("Testing where with Vector3D");

    using Vector3D = std::array<double, 3>;

    // Test with boolean condition
    Serie<bool> condition{true, false, true};

    Serie<Vector3D> then_serie{Vector3D{1.0, 1.0, 1.0}, Vector3D{2.0, 2.0, 2.0},
                               Vector3D{3.0, 3.0, 3.0}};

    Serie<Vector3D> else_serie{Vector3D{-1.0, -1.0, -1.0},
                               Vector3D{-2.0, -2.0, -2.0},
                               Vector3D{-3.0, -3.0, -3.0}};

    auto result = where(condition, then_serie, else_serie);

    // Verify results
    EXPECT_EQ(result.size(), 3);
    EXPECT_ARRAY_NEAR(result[0], Vector3D({1.0, 1.0, 1.0}), 1e-10);
    EXPECT_ARRAY_NEAR(result[1], Vector3D({-2.0, -2.0, -2.0}), 1e-10);
    EXPECT_ARRAY_NEAR(result[2], Vector3D({3.0, 3.0, 3.0}), 1e-10);

    // Test pipeline operation
    auto result_pipe = condition | bind_where(then_serie, else_serie);
    EXPECT_EQ(result_pipe.size(), 3);
    EXPECT_ARRAY_NEAR(result_pipe[0], Vector3D({1.0, 1.0, 1.0}), 1e-10);
    EXPECT_ARRAY_NEAR(result_pipe[1], Vector3D({-2.0, -2.0, -2.0}), 1e-10);
    EXPECT_ARRAY_NEAR(result_pipe[2], Vector3D({3.0, 3.0, 3.0}), 1e-10);
}

TEST(Serie, WhereErrors) {
    MSG("Testing where error conditions");

    Serie<bool> condition{true, false, true};
    Serie<int> then_serie{1, 2}; // Wrong size
    Serie<int> else_serie{10, 20, 30};

    EXPECT_THROW(where(condition, then_serie, else_serie), std::runtime_error);
    EXPECT_THROW(where(condition, else_serie, then_serie), std::runtime_error);
}

RUN_TESTS()