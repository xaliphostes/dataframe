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
//#include <concepts>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <random>
#include <type_traits>

namespace df {

/**
* @brief Available probability distribution types for random number
generation
*/
enum class DistType {
    Uniform,   ///< Uniform distribution (continuous or discrete)
    Normal,    ///< Normal (Gaussian) distribution
    Bernoulli, ///< Bernoulli distribution
    Poisson,   ///< Poisson distribution
    Sampling   ///< Random sampling from a population
};

// -------------------------------------------------------------

/**
 * @brief Base template for distribution parameters
 * @tparam D Distribution type
 */
template <typename T, DistType D> struct DistParams;

// -------------------------------------------------------------

// Generic random function
template <typename T, DistType D>
Serie<T> random(size_t n, const DistParams<T, D> &params);

// Specializations for convenience
template <typename T> Serie<T> random_uniform(size_t, T, T);
template <typename T> Serie<T> random_normal(size_t, T, T);
template <typename T> Serie<T> random_bernoulli(size_t, double);
template <typename T> Serie<T> random_poisson(size_t, double);
template <typename T> Serie<T> random_sampling(size_t, const std::vector<T>& , T);

} // namespace df

#include "inline/random.hxx"