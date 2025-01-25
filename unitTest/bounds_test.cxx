
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
#include <dataframe/functional/math/bounds.h>

TEST(minMax, basic) {
    df::GenSerie<double> s(3, {
        1.0, 2.0, 3.0,  // First vector
        0.5, 4.0, 1.0,  // Second vector
        2.0, 1.0, 5.0   // Third vector
    });

    auto bounds = df::math::bounds(s);
    
    EXPECT_ARRAY_EQ(bounds.min, std::vector<double>({0.5, 1.0, 1.0}));
    EXPECT_ARRAY_EQ(bounds.max, std::vector<double>({2.0, 4.0, 5.0}));
}

TEST(minMax, empty_serie) {
    df::GenSerie<double> empty;
    auto bounds = df::math::bounds(empty);
    
    EXPECT_TRUE(bounds.min.empty());
    EXPECT_TRUE(bounds.max.empty());
}

RUN_TESTS()