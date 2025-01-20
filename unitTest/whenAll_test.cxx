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
#include <dataframe/functional/reduce.h>
#include <dataframe/functional/utils/whenAll.h>

TEST(whenAll, test1) {
    // Create test series
    df::GenSerie<double> s1(3, {1,2,3, 4,5,6});
    df::GenSerie<double> s2(3, {7,8,9, 10,11,12});
    df::GenSerie<double> s3(3, {13,14,15, 16,17,18});

    // Test parallel transformation
    auto result1 = df::utils::whenAll<double>(
        [](const df::GenSerie<double>& s) { return s; },
        {s1, s2, s3}
    );

    // Test parallel execution
    auto [r1, r2, r3] = df::utils::whenAll<double>(s1, s2, s3);

    // Verify results
    EXPECT_NEAR(r1.value(0), 1.0, 1e-10);
    EXPECT_NEAR(r2.value(0), 7.0, 1e-10);
    EXPECT_NEAR(r3.value(0), 13.0, 1e-10);
}

// Test error cases
// TEST(whenAll, errors) {
//     df::GenSerie<double> s1(3, {1,2,3});
//     df::GenSerie<float> s2(3, {4,5,6});  // Different type
    
//     // This should not compile due to different types
//     // auto [r1, r2] = df::utils::whenAll<double>(s1, s2);
//     EXPECT_THROW(df::utils::whenAll<double>(s1, s2), std::invalid_argument);
// }

RUN_TESTS()
