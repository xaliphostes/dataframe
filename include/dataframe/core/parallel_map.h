/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Apply a function to each element of a Serie in parallel.
 * 
 * This function distributes the workload across multiple threads to improve performance
 * for large datasets. For small datasets (less than 1000 elements), it automatically
 * falls back to the regular sequential map operation for better efficiency.
 * 
 * The number of threads used is determined automatically based on:
 * - Available hardware concurrency (number of logical cores)
 * - Dataset size (to avoid thread overhead for small datasets)
 * - A minimum workload per thread (to ensure efficient parallelization)
 * 
 * @tparam F The type of the function to apply
 * @tparam T The element type of the input Serie
 * @param callback The function to apply to each element. It should take the element value
 *                 and its index as parameters and return a new value.
 * @param serie The input Serie to process
 * @return A new Serie containing the results of applying the function to each element
 * 
 * @example
 * ```cpp
 * // Create a large Serie of numbers
 * df::Serie<double> values = df::range(0, 1000000);
 * 
 * // Apply a computationally intensive function in parallel
 * auto results = df::parallel_map([](double x, size_t idx) {
 *     // Simulate complex computation
 *     double result = 0;
 *     for (int i = 0; i < 1000; ++i) {
 *         result += std::sin(x + i);
 *     }
 *     return result;
 * }, values);
 * 
 * // Performance comparison with sequential map
 * auto start_time = std::chrono::high_resolution_clock::now();
 * auto parallel_results = df::parallel_map([](double x, size_t) { return std::sqrt(x); }, values);
 * auto parallel_time = std::chrono::high_resolution_clock::now() - start_time;
 * 
 * start_time = std::chrono::high_resolution_clock::now();
 * auto sequential_results = values.map([](double x, size_t) { return std::sqrt(x); });
 * auto sequential_time = std::chrono::high_resolution_clock::now() - start_time;
 * 
 * std::cout << "Parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(parallel_time).count() << " ms\n";
 * std::cout << "Sequential time: " << std::chrono::duration_cast<std::chrono::milliseconds>(sequential_time).count() << " ms\n";
 * ```
 */
template <typename F, typename T>
auto parallel_map(F &&callback, const Serie<T> &serie);

/**
 * @brief Apply a function to elements from multiple Series in parallel.
 * 
 * This function provides parallel processing for operations that need to combine
 * elements from multiple Series. It applies the given function to corresponding
 * elements from each Serie at the same index position.
 * 
 * Similar to the single-Serie version, this function:
 * - Automatically determines the optimal number of threads to use
 * - Falls back to sequential processing for small datasets
 * - Distributes the workload evenly across available threads
 * 
 * @note All input Series must have the same size
 * 
 * @tparam F The type of the function to apply
 * @tparam T The element type of the first input Serie
 * @tparam Args The element types of additional input Series
 * @param callback The function to apply to each set of elements. It should take elements
 *                 from each Serie, plus the index, and return a new value.
 * @param first The first input Serie
 * @param second The second input Serie
 * @param args Additional input Series
 * @return A new Serie containing the results of applying the function
 * 
 * @example
 * ```cpp
 * // Create three Series of the same size
 * df::Serie<double> xs = df::range(0, 1000000);
 * df::Serie<double> ys = xs.map([](double x, size_t) { return x * 2; });
 * df::Serie<double> zs = xs.map([](double x, size_t) { return x * 3; });
 * 
 * // Calculate Euclidean distance in parallel
 * auto distances = df::parallel_map([](double x, double y, double z, size_t) {
 *     return std::sqrt(x*x + y*y + z*z);
 * }, xs, ys, zs);
 * 
 * // Calculate weighted sum in parallel
 * df::Serie<double> weights{0.5, 0.3, 0.2};
 * auto weighted_sums = df::parallel_map([](double x, double y, double z, size_t) {
 *     return x * 0.5 + y * 0.3 + z * 0.2;
 * }, xs, ys, zs);
 * ```
 */
template <typename F, typename T, typename... Args>
auto parallel_map(F &&callback, const Serie<T> &first, const Serie<T> &second,
                  const Args &...args);

/**
 * @brief Create a bindable function for parallel map operations in a pipeline.
 * 
 * This function creates a callable object that can be used with the pipe operator
 * to apply parallel mapping operations in a functional pipeline.
 * 
 * @tparam F The type of the function to apply
 * @param callback The function to apply to each element
 * @return A callable that accepts a Serie and applies the callback function in parallel
 * 
 * @example
 * ```cpp
 * // Create a data processing pipeline with parallel operations
 * auto process_data = [](const df::Serie<double>& input) {
 *     return input
 *         | df::bind_map([](double x, size_t) { return x + 10; })  // Sequential
 *         | df::bind_parallel_map([](double x, size_t) {           // Parallel (expensive)
 *             double result = 0;
 *             for (int i = 0; i < 1000; ++i) {
 *                 result += std::sin(x * i);
 *             }
 *             return result;
 *         })
 *         | df::bind_map([](double x, size_t) { return std::sqrt(x); });  // Sequential
 * };
 * 
 * // Apply the pipeline to data
 * df::Serie<double> data = df::random_uniform<double>(1000000, 0, 100);
 * auto result = process_data(data);
 * ```
 */
template <typename F> auto bind_parallel_map(F &&callback);

/**
 * @brief Shorthand alias for parallel_map.
 * 
 * This function provides a shorter name for the parallel_map function
 * while maintaining the same functionality.
 * 
 * @tparam F The type of the function to apply
 * @tparam T The element type of the first input Serie
 * @tparam Args The element types of additional input Series
 * @param callback The function to apply to each element or set of elements
 * @param first The first input Serie
 * @param args Additional input Series (optional)
 * @return A new Serie containing the results of applying the function
 * 
 * @example
 * ```cpp
 * // Using the shorthand alias
 * df::Serie<double> values = df::range(0, 1000000);
 * auto results = df::par_map([](double x, size_t) { return x * x; }, values);
 * 
 * // With multiple input Series
 * df::Serie<double> xs = df::range(0, 1000000);
 * df::Serie<double> ys = df::range(1000000, 2000000);
 * auto sums = df::par_map([](double x, double y, size_t) { return x + y; }, xs, ys);
 * ```
 */
template <typename F, typename T, typename... Args>
auto par_map(F &&callback, const Serie<T> &first, const Args &...args);

} // namespace df

#include "inline/parallel_map.hxx"
