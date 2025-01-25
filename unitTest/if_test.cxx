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
#include <dataframe/functional/conditional/if.h>
#include <dataframe/functional/math/negate.h>

// Test scalar if operation
TEST(If, ScalarOperation) {
    // Create condition: x > 0
    std::vector<double> values = {-1.0, 2.0, -3.0, 4.0};
    std::vector<double> conditions = {0.0, 1.0, 0.0, 1.0};
    
    df::GenSerie<double> condition(1, conditions);
    df::GenSerie<double> series(1, values);
    
    // Test with constant values
    auto result1 = df::cond::if_fn(condition, 1.0, -1.0);
    std::vector<double> expected1 = {-1.0, 1.0, -1.0, 1.0};
    EXPECT_ARRAY_EQ(result1.asArray(), expected1);
    
    // Test with series
    auto result2 = df::cond::if_fn(condition, series, df::math::negate(series));
    auto result3 = df::cond::if_fn(condition, series, -series);
    std::vector<double> expected2 = {1.0, 2.0, 3.0, 4.0};
    EXPECT_ARRAY_EQ(result2.asArray(), expected2);
    EXPECT_ARRAY_EQ(result3.asArray(), expected2);
}

// Test vector if operation
TEST(If, VectorOperation) {
    // Create condition
    std::vector<double> conditions = {0.0, 1.0};
    
    // Create 2D vectors
    std::vector<double> true_values = {1.0, 0.0, 2.0, 0.0};
    std::vector<double> false_values = {0.0, 1.0, 0.0, 2.0};
    
    df::GenSerie<double> condition(1, conditions);
    df::GenSerie<double> true_serie(2, true_values);
    df::GenSerie<double> false_serie(2, false_values);
    
    auto result = df::cond::if_fn(condition, true_serie, false_serie);
    
    // Check dimensions
    EXPECT_EQ(result.itemSize(), 2);
    EXPECT_EQ(result.count(), 2);
    
    // Check values
    auto vec1 = result.array(0);
    auto vec2 = result.array(1);
    
    EXPECT_ARRAY_EQ(vec1, std::vector<double>({0.0, 1.0}));
    EXPECT_ARRAY_EQ(vec2, std::vector<double>({2.0, 0.0}));
}

// Test error conditions
TEST(If, ErrorHandling) {
    std::vector<double> conditions = {0.0, 1.0};
    std::vector<double> values1 = {1.0, 2.0};
    std::vector<double> values2 = {1.0, 2.0, 3.0};  // Wrong count
    std::vector<double> vector_cond = {0.0, 1.0, 0.0, 1.0};  // Wrong itemSize
    
    df::GenSerie<double> condition(1, conditions);
    df::GenSerie<double> series1(1, values1);
    df::GenSerie<double> series2(1, values2);
    df::GenSerie<double> bad_condition(2, vector_cond);
    
    // Test mismatched counts
    EXPECT_THROW(
        df::cond::if_fn(condition, series1, series2),
        std::invalid_argument
    );
    
    // Test vector condition
    EXPECT_THROW(
        df::cond::if_fn(bad_condition, series1, series1),
        std::invalid_argument
    );
}

// Test make_if wrapper
TEST(If, WrapperFunction) {
    std::vector<double> conditions = {0.0, 1.0, 0.0, 1.0};
    df::GenSerie<double> condition(1, conditions);
    
    // Create wrapper with constant values
    auto if_op = df::cond::make_if(1.0, -1.0);
    auto result = if_op(condition);
    
    std::vector<double> expected = {-1.0, 1.0, -1.0, 1.0};
    EXPECT_ARRAY_EQ(result.asArray(), expected);
}

// Add main function to run all tests
RUN_TESTS()
