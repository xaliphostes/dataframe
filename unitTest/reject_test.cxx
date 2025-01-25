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
#include <dataframe/functional/utils/reject.h>

TEST(Reject, Scalar) {
    df::GenSerie<double> s(1, {1.0, 2.0, 3.0, 4.0, 5.0});

    MSG("Reject values > 3");
    auto result = df::utils::reject([](double x, uint32_t) { return x > 3.0; }, s);
    EXPECT_EQ(result.count(), 3);
    EXPECT_EQ(result.value(0), 1.0);
    EXPECT_EQ(result.value(1), 2.0);
    EXPECT_EQ(result.value(2), 3.0);

    MSG("Reject with index");
    auto withIndex =
        df::utils::reject([](double x, uint32_t i) { return x > 3.0 || i >= 2; }, s);
    EXPECT_EQ(withIndex.count(), 2);
    EXPECT_EQ(withIndex.value(0), 1.0);
    EXPECT_EQ(withIndex.value(1), 2.0);

    MSG("Reject all");
    auto none = df::utils::reject([](double, uint32_t) { return true; }, s);
    EXPECT_EQ(none.count(), 0);

    MSG("Reject none");
    auto all = df::utils::reject([](double, uint32_t) { return false; }, s);
    EXPECT_EQ(all.count(), s.count());
    EXPECT_ARRAY_EQ(all.asArray(), s.asArray());
}

TEST(Reject, Vector) {
    df::GenSerie<double> vectors(3, {
                                        1.0, 0.0, 0.0, // vec1 with length 1
                                        2.0, 2.0, 0.0, // vec2 with length 2.83
                                        0.0, 3.0, 0.0  // vec3 with length 3
                                    });

    MSG("Reject by vector length");
    auto result = df::utils::reject(
        [](const std::vector<double> &v, uint32_t) {
            double lenSq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
            return lenSq >= 3.0;
        },
        vectors);

    EXPECT_EQ(result.count(), 1);
    EXPECT_ARRAY_EQ(result.array(0), std::vector<double>({1.0, 0.0, 0.0}));

    MSG("Reject with index");
    auto withIndex = df::utils::reject(
        [](const std::vector<double> &, uint32_t i) { return i != 1; },
        vectors);

    EXPECT_EQ(withIndex.count(), 1);
    EXPECT_ARRAY_EQ(withIndex.array(0), std::vector<double>({2.0, 2.0, 0.0}));
}

TEST(Reject, Error) {
    df::GenSerie<double> scalar(1, {1.0, 2.0});
    df::GenSerie<double> vector(3, {1.0, 0.0, 0.0});

    MSG("Scalar predicate with vector serie should throw");
    EXPECT_THROW(df::utils::reject([](double x, uint32_t) { return x > 1.0; }, vector),
                 std::invalid_argument);

    // MSG("Vector predicate with scalar serie should throw");
    // EXPECT_THROW(
    //     df::utils::reject([](const std::vector<double> &, uint32_t) { return true; },
    //                scalar),
    //     std::invalid_argument);
}

RUN_TESTS()