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
#include <chrono>
#include <concepts>
#include <dataframe/Serie.h>
#include <dataframe/utils.h>
#include <random>
#include <type_traits>

namespace df {

// ----------------------------------------------------------------------------

/**
 * @brief Available probability distribution types for random number generation
 */
enum class DistType {
    Uniform,   ///< Uniform distribution (continuous or discrete)
    Normal,    ///< Normal (Gaussian) distribution
    Bernoulli, ///< Bernoulli distribution
    Poisson,   ///< Poisson distribution
    Sampling   ///< Random sampling from a population
};

// ----------------------------------------------------------------------------

/**
 * @brief Base template for distribution parameters
 * @tparam D Distribution type
 */
template <DistType D> struct DistParams {};

/**
 * @brief Generates random numbers uniformly distributed between min and max.
 * For continuous distributions (floating-point types), generates values in
 * [min, max). For discrete distributions (integral types), generates values in
 * [min, max].
 *
 * @tparam T Type of numbers to generate (integral or floating-point)
 * @param n Number of values to generate
 * @param min Minimum value (inclusive)
 * @param max Maximum value (inclusive for integral types, exclusive for
 * floating-point)
 * @return Serie<T> containing the generated values
 *
 * Example:
 * @code
 * // Generate 100 uniform random doubles between 0 and 1
 * auto uniform = df::random_uniform<double>(100, 0.0, 1.0);
 *
 * // Generate 100 uniform random integers between 1 and 6 (dice rolls)
 * auto dice = df::random_uniform<int>(100, 1, 6);
 * @endcode
 */
template <typename T> Serie<T> random_uniform(size_t n, T min = 0, T max = 1);

/**
 * @brief Generates random numbers according to the normal distribution with
 * specified mean (μ) and standard deviation (σ). The probability density
 * function is: f(x) = (1 / (σ√(2π))) * e^(-(x-μ)²/(2σ²))
 *
 * @tparam T Type of numbers to generate (typically floating-point)
 * @param n Number of values to generate
 * @param mean Mean (μ) of the distribution
 * @param stddev Standard deviation (σ) of the distribution
 * @return Serie<T> containing the generated values
 *
 * Example:
 * @code
 * // Generate 100 normally distributed values with mean 0 and std dev 1
 * auto normal = df::random_normal<double>(100, 0.0, 1.0);
 *
 * // Generate IQ scores (mean 100, std dev 15)
 * auto iq_scores = df::random_normal<double>(1000, 100.0, 15.0);
 * @endcode
 */
template <typename T>
Serie<T> random_normal(size_t n, T mean = 0, T stddev = 1);

/**
 * @brief Generates random boolean values (0 or 1) with a specified probability
 * of success (1). This is equivalent to simulating a biased coin flip, where
 * probability represents the chance of getting heads (1).
 *
 * @tparam T Type of numbers to generate (typically int or bool)
 * @param n Number of values to generate
 * @param probability Probability of generating 1 (success)
 * @return Serie<T> containing the generated values
 *
 * Example:
 * @code
 * // Simulate 100 fair coin flips (50% chance of heads)
 * auto fair_flips = df::random_bernoulli<int>(100, 0.5);
 *
 * // Simulate 100 biased coin flips (70% chance of heads)
 * auto biased_flips = df::random_bernoulli<int>(100, 0.7);
 * @endcode
 */
template <typename T>
Serie<T> random_bernoulli(size_t n, double probability = 0.5);

/**
 * @brief Generates random integers according to the Poisson distribution with
 * specified mean. The Poisson distribution models the number of events
 * occurring in a fixed time interval when these events occur with a known
 * average rate and independently.
 *
 * @tparam T Type of numbers to generate (typically int)
 * @param n Number of values to generate
 * @param mean Mean (λ) of the distribution
 * @return Serie<T> containing the generated values
 *
 * Example:
 * @code
 * // Model customers arriving at a store (average 5 per hour)
 * auto arrivals = df::random_poisson<int>(24, 5.0);  // 24 hours of data
 *
 * // Model defects in material (average 2 per square meter)
 * auto defects = df::random_poisson<int>(100, 2.0);  // 100 samples
 * @endcode
 */
template <typename T> Serie<T> random_poisson(size_t n, double mean = 1.0);

/**
 * @brief Performs random sampling from a given population, either with or
 * without replacement. With replacement means the same value can be selected
 * multiple times. Without replacement means each value can be selected only
 * once.
 *
 * @tparam T Type of numbers to generate (must match population type)
 * @param n Number of values to generate
 * @param population Vector of values to sample from
 * @param replacement Whether to sample with replacement (true) or without
 * (false)
 * @return Serie<T> containing the generated values
 *
 * Example:
 * @code
 * // Population of values to sample from
 * std::vector<double> population = {1.0, 2.0, 3.0, 4.0, 5.0};
 *
 * // Draw 10 samples with replacement (like rolling a die multiple times)
 * auto with_replacement = df::random_sample<double>(10, population, true);
 *
 * // Draw 3 samples without replacement (like drawing cards from a deck)
 * auto without_replacement = df::random_sample<double>(3, population, false);
 * @endcode
 */
template <typename T>
Serie<T> random_sample(size_t n, const std::vector<T> &population,
                       bool replacement = true);

/**
 * @brief Generic random number generation function
 * @tparam T Type of numbers to generate
 * @tparam D Distribution type from DistType enum
 * @param n Number of values to generate
 * @param params Parameters for the specified distribution
 * @return Serie<T> containing the generated values
 */
template <typename T, DistType D>
Serie<T> random(size_t n, const DistParams<D> &params);

} // namespace df

#include "inline/random.hxx"