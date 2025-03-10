// In a new file switch.h:
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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <functional>
#include <utility>
#include <vector>

namespace df {

/**
 * @brief Applies different transformations to elements based on conditions.
 *
 * The switch function evaluates a series of conditions for each element in the
 * input Serie, and applies the first matching transformation. If no condition
 * matches, a default transformation is applied.
 *
 * @tparam T Type of elements in the input Serie
 * @tparam R Type of elements in the output Serie
 * @param serie Input Serie to process
 * @param cases Vector of condition-transformation pairs
 * @param default_case Default transformation to apply when no condition matches
 * @return Serie<R> A new Serie with transformed elements
 *
 * @code
 * // Convert grades to letter grades
 * Serie<double> scores = {95, 82, 67, 45, 78};
 * auto letter_grades = switch_case(scores,
 *     {{[](double x) { return x >= 90; }, [](double) { return "A"; }},
 *      {[](double x) { return x >= 80; }, [](double) { return "B"; }},
 *      {[](double x) { return x >= 70; }, [](double) { return "C"; }},
 *      {[](double x) { return x >= 60; }, [](double) { return "D"; }}},
 *     [](double) { return "F"; });
 *
 * // Using pipe syntax
 * auto letters = scores | bind_switch_case(
 *     {{[](double x) { return x >= 90; }, [](double) { return "A"; }},
 *      {[](double x) { return x >= 80; }, [](double) { return "B"; }},
 *      {[](double x) { return x >= 70; }, [](double) { return "C"; }},
 *      {[](double x) { return x >= 60; }, [](double) { return "D"; }}},
 *     [](double) { return "F"; });
 * @endcode
 */
template <typename T, typename R>
Serie<R>
switch_case(const Serie<T> &serie,
            const std::vector<std::pair<std::function<bool(const T &)>,
                                        std::function<R(const T &)>>> &cases,
            std::function<R(const T &)> default_case);

/**
 * @brief Create a resusable switch_case pipeline operation
 * @code
 * auto pipeline = df::make_pipe(
 *         df::bind_map([](int x, size_t) { return x * 2; }),
 *         df::bind_switch_case<int, std::string>(...));
 *
 * auto result1 = pipeline(series1);
 * auto result2 = pipeline(series2);
 * auto result3 = pipeline(series3);
 * @endcode
 */
template <typename T, typename R>
auto bind_switch_case(
    const std::vector<std::pair<std::function<bool(const T &)>,
                                std::function<R(const T &)>>> &cases,
    std::function<R(const T &)> default_case);

/**
 * @brief Helper to create a bound switch operation using MAKE_OP
 */
MAKE_OP(switch_case);

} // namespace df

#include "inline/switch.hxx"