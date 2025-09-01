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
#include <dataframe/math/add.h>
#include <dataframe/math/mult.h>
#include <stdexcept>
#include <type_traits>

namespace df {

    // ================================================================
    //                              Helpers

    template <typename T> struct weighted_sum_binder_scalar;
    template <typename T> struct weighted_sum_binder_series;

    template <typename T> using InitializerSeries = std::initializer_list<Serie<T>>;

    // Remove the arithmetic constraint - these will work for any type supporting mult/add
    template <typename T> using InitializerBinderScalar = weighted_sum_binder_scalar<T>;

    template <typename T> using InitializerBinderSeries = weighted_sum_binder_series<T>;

    // ================================================================

    /**
     * @brief Base weightedSum implementation that works with vectors
     * Works with any type T that supports multiplication and addition operations
     */
    template <typename T, typename W>
    auto weightedSum(const std::vector<Serie<T>>&, const std::vector<W>&) -> Serie<T>;

    template <typename T, typename W>
    auto weightedSum(const InitializerSeries<T>&, const std::vector<W>&) -> Serie<T>;

    // WeightedSum with series weights
    template <typename T, typename W>
    auto weightedSum(const std::vector<Serie<T>>&, const std::vector<Serie<W>>&) -> Serie<T>;

    template <typename T, typename W>
    auto weightedSum(const InitializerSeries<T>&, const std::initializer_list<Serie<W>>&)
        -> Serie<T>;

    // ================================================================
    // Specific overloads for common arithmetic weight types to fix deduction
    // ================================================================

    template <typename T>
    auto weightedSum(const std::vector<Serie<T>>&, const ArrayType<T>&) -> Serie<T>;

    template <typename T>
    auto weightedSum(const InitializerSeries<T>&, const ArrayType<T>&) -> Serie<T>;

    template <typename T>
    auto weightedSum(const InitializerSeries<T>&, const std::initializer_list<int>&) -> Serie<T>;

    template <typename T>
    auto weightedSum(const InitializerSeries<T>&, const std::initializer_list<float>&) -> Serie<T>;

    template <typename T>
    auto weightedSum(const InitializerSeries<T>&, const std::initializer_list<double>&) -> Serie<T>;

    /**
     * @brief Helper functions to create binders
     */
    template <typename T, typename W>
    auto bind_weightedSum(const InitializerSeries<T>&, const std::vector<W>&)
        -> InitializerBinderScalar<T>;

    template <typename T, typename W>
    auto bind_weightedSum(const InitializerSeries<T>&, const std::initializer_list<Serie<W>>&)
        -> InitializerBinderSeries<T>;

} // namespace df

#include "inline/weightedSum2.hxx"