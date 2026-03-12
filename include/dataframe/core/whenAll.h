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

#pragma once
#include <dataframe/Serie.h>

namespace df {

    /**
     * @brief Execute transformations on multiple Series in parallel.
     *
     * The implementation provides:
     * - Parallel execution of transformations on multiple Series
     * - Two versions: one for transformations returning concatenated results, one
     * for tuple returns
     * - Type safety through static assertions
     * - Error handling through futures
     * - Resource management through RAII
     * - Performance testing for heavy computations
     *
     * @code
     * // Example usage with transformation:
     * df::Serie<double> s1{1.0, 2.0, 3.0};
     * df::Serie<double> s2{4.0, 5.0, 6.0};
     *
     * auto result = df::whenAll([](const Serie<double>& s) {
     *     return s.map([](double x) { return x * 2; });
     * }, {s1, s2});
     *
     * // Example usage without transformation:
     * auto [r1, r2] = df::whenAll(s1, s2);
     * @endcode
     */
    template <typename T, typename F>
    Serie<T> whenAll(F&& transform, const std::vector<Serie<T>>& series);

    /**
     * The main purposes are:
     * - Parallel Copy/Load: When you have multiple Series and want to load or copy
     * them in parallel rather than sequentially. This is useful when series are
     * large, when series might be coming from different sources/storage, or when
     * you want to maintain the separation of Series rather than concatenating them
     * - Maintaining Series Identity: Unlike the transformation version which
     * concatenates results, this version keeps each Serie separate in the tuple,
     * preserving their individual identity and order.
     */
    template <typename T, typename... Series> auto whenAll(const Series&... series);

    /**
     * @brief Create a bind_whenAll function for use in pipelines
     *
     * @code
     * // Example usage:
     * df::Serie<double> s1{1.0, 2.0, 3.0};
     * df::Serie<double> s2{4.0, 5.0, 6.0};
     *
     * // Using bind_whenAll in a pipeline with transformation
     * auto transform = [](const Serie<double>& s) { return s.map([](double x) {
     * return x * 2; }); }; auto result1 = s1 | bind_whenAll(transform, {s2});
     *
     * // Using bind_whenAll to get tuple result
     * auto result2 = s1 | bind_whenAll(s2, s3);
     * @endcode
     */

    // Version for transformation
    template <typename F, typename T>
    auto bind_whenAll(F&& transform, const std::vector<Serie<T>>& series);

    // Version without transformation (returns tuple)
    template <typename T, typename... Series> auto bind_whenAll(const Series&... series);

} // namespace df

#include "inline/whenAll.hxx"