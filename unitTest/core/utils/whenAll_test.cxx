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
 */

#include "../../TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/map.h>
#include <dataframe/utils/whenAll.h>
#include <limits>

TEST(whenAll, with_transformation) {
    df::Serie<double> s1{1.0, 2.0, 3.0};
    df::Serie<double> s2{4.0, 5.0, 6.0};

    auto result = df::whenAll(
        [](const df::Serie<double> &s) {
            return s.map([](double x, size_t) { return x * 2; });
        },
        std::vector{s1, s2});

    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(result[0] == 2.0); // 1.0 * 2
    EXPECT_TRUE(result[3] == 8.0); // 4.0 * 2
}

TEST(whenAll, without_transformation) {
    df::Serie<int> s1{1, 2, 3};
    df::Serie<int> s2{4, 5, 6};

    auto [r1, r2] = df::whenAll<int>(s1, s2);

    EXPECT_TRUE(r1.size() == 3);
    EXPECT_TRUE(r2.size() == 3);
    EXPECT_TRUE(r1[0] == 1);
    EXPECT_TRUE(r2[0] == 4);
}

TEST(whenAll, empty_series) {
    df::Serie<double> s1;
    df::Serie<double> s2;

    auto result = df::whenAll([](const df::Serie<double> &s) { return s; },
                              std::vector{s1, s2});

    EXPECT_TRUE(result.size() == 0);
}

TEST(whenAll, heavy_computation) {
    // Create large series for meaningful parallel execution
    std::vector<double> data1(100000, 1.0);
    std::vector<double> data2(100000, 2.0);

    df::Serie<double> s1(data1);
    df::Serie<double> s2(data2);

    auto heavy_transform = [](const df::Serie<double> &s) {
        return s.map([](double x, size_t) {
            // Simulate heavy computation
            double result = 0;
            for (int i = 0; i < 1000; ++i) {
                result += std::sin(x) * std::cos(x);
            }
            return result;
        });
    };

    auto start = std::chrono::high_resolution_clock::now();
    auto result = df::whenAll(heavy_transform, std::vector{s1, s2});
    auto duration = std::chrono::high_resolution_clock::now() - start;

    std::cout << "Parallel execution time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                     .count()
              << "ms\n";

    EXPECT_TRUE(result.size() == 200000);
}

TEST(bind_whenAll, with_transformation) {
    df::Serie<double> s1{1.0, 2.0, 3.0};
    df::Serie<double> s2{4.0, 5.0, 6.0};

    auto transform = [](const df::Serie<double> &s) {
        return s.map([](double x, size_t) { return x * 2; });
    };

    auto result = s1 | df::bind_whenAll(transform, std::vector{s2});

    EXPECT_TRUE(result.size() == 6);
    EXPECT_TRUE(result[0] == 2.0); // 1.0 * 2
    EXPECT_TRUE(result[3] == 8.0); // 4.0 * 2
}

TEST(bind_whenAll, without_transformation) {
    df::Serie<int> s1{1, 2, 3};
    df::Serie<int> s2{4, 5, 6};
    df::Serie<int> s3{7, 8, 9};

    auto [r1, r2, r3] = s1 | df::bind_whenAll<int>(s2, s3);

    EXPECT_TRUE(r1.size() == 3);
    EXPECT_TRUE(r2.size() == 3);
    EXPECT_TRUE(r3.size() == 3);
    EXPECT_TRUE(r1[0] == 1);
    EXPECT_TRUE(r2[0] == 4);
    EXPECT_TRUE(r3[0] == 7);
}

TEST(bind_whenAll, chaining) {
    df::Serie<double> s1{1.0, 2.0};
    df::Serie<double> s2{3.0, 4.0};

    auto transform = [](const df::Serie<double> &s) {
        return s.map([](double x, size_t) { return x + 1; });
    };

    // Chain multiple operations
    auto result = s1 | df::bind_map([](double x, size_t) { return x * 2; }) |
                  df::bind_whenAll(transform, std::vector{s2});

    EXPECT_TRUE(result.size() == 4);
    EXPECT_TRUE(result[0] == 3.0); // (1.0 * 2) + 1
    EXPECT_TRUE(result[2] == 4.0); // 3.0 + 1
}

RUN_TESTS();