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

#include "TEST.h"
#include <dataframe/functional/algebra/norm.h>

TEST(norm, make_norm) {
    double epsilon = 1e-10;
    // Test scalar series
    df::GenSerie<double> s1(1, {-1, 2, -3, 4});
    auto r1 = df::pipe(s1, df::algebra::make_norm());  // Should give {1, 2, 3, 4}

    // Test vector series
    df::GenSerie<double> s2(3, {3, 0, 0,  // vector of length 3
                               0, 4, 0,    // vector of length 4
                               0, 0, 5});  // vector of length 5
    auto r2 = df::pipe(s2, df::algebra::make_norm());  // Should give {3, 4, 5}

    // Test with template parameter explicitly specified
    auto r3 = df::pipe(s1, df::algebra::make_norm<double>());

    // Verify results
    EXPECT_NEAR(r1.value(0), 1.0, epsilon);
    EXPECT_NEAR(r1.value(1), 2.0, epsilon);
    EXPECT_NEAR(r1.value(2), 3.0, epsilon);
    EXPECT_NEAR(r1.value(3), 4.0, epsilon);

    EXPECT_NEAR(r2.value(0), 3.0, epsilon);
    EXPECT_NEAR(r2.value(1), 4.0, epsilon);
    EXPECT_NEAR(r2.value(2), 5.0, epsilon);
}

RUN_TESTS()