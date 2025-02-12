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
#include <dataframe/Serie.h>
#include <dataframe/forEach.h>
#include <dataframe/map.h>
#include <dataframe/pipe.h>
#include <dataframe/reduce.h>

auto series1 = df::Serie<int>{1, 2, 3, 4, 5};
auto series2 = df::Serie<int>{6, 7, 8, 9, 10};

// ------------------------------------------------

TEST(pipe, pipe) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5};

    // Chain multiple operations
    auto result =
        series | df::bind_map([](int x, int) { return x * 2; }) |
        df::bind_reduce([](int acc, int x, int) { return acc + x; }, 0); // 30

    std::cerr << result << std::endl;
}

// TEST(pipe, pipe_2) {
//     // Multiple series operations
//     auto result2 =
//         series1 |
//         df::bind_map([](int x, int y, int) { return x * y; }) |
//         df::bind_reduce([](int acc, int x, int) { return acc + x; }, 0);

//     // Using forEach in a pipe
//     series1 | df::bind_map([](int x, int) { return x * 2; }) |
//         df::bind_forEach([](int x, int) { std::cout << x << " "; });
// }

TEST(pipe, make_pipe) {
    auto series = df::Serie<int>{1, 2, 3, 4, 5};

    // Create a pipeline that doubles numbers and then sums them
    auto pipeline = df::make_pipe(
        df::bind_map([](int x, int) { return x * 2; }),
        df::bind_reduce([](int acc, int x, int) { return acc + x; }, 0));

    // Use the pipeline
    auto result = pipeline(series);
    std::cerr << result << std::endl;

    // // Using compose (operations are applied from right to left)
    // auto pipeline2 = df::compose(
    //     df::bind_reduce([](int acc, int x, int) { return acc + x; }, 0),
    //     df::bind_map([](int x, int) { return x * 2; }));

    // Can also be used directly with the pipe operator
    auto result2 = series | pipeline;
    std::cerr << result2 << std::endl;

    // Complex example with multiple operations
    auto complex_pipeline = df::make_pipe(
        df::bind_map([](int x, int) { return x * 2; }),
        df::bind_map([](int x, int) { return x + 1; }),
        df::bind_reduce([](int acc, int x, int) { return acc + x; }, 0));

    auto result3 = complex_pipeline(series);
    std::cerr << result3 << std::endl;
}

RUN_TESTS();