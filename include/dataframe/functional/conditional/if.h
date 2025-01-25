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
#include "../../Serie.h"

namespace df {
namespace cond {

/**
 * @brief Conditional selection between two series based on a condition
 * @param condition Condition serie (itemSize 1)
 * @param true_serie Serie to select when condition is true
 * @param false_serie Serie to select when condition is false
 * @return GenSerie<T> Result with values selected based on condition
 * @code
 * // With constant values
 * auto result = df::if_fn(condition, 1.0, 0.0);
 * 
 * // With series
 * auto result = df::if_fn(condition, true_serie, false_serie);
 * 
 * // With wrapper
 * auto if_op = df::make_if(1.0, 0.0);
 * auto result = if_op(condition);
 * @endcode
 */
template <typename T>
GenSerie<T> if_fn(const GenSerie<T> &condition, const GenSerie<T> &true_serie,
                  const GenSerie<T> &false_serie) {
    // Validate inputs
    if (condition.itemSize() != 1) {
        throw std::invalid_argument("Condition must be scalar (itemSize=1)");
    }
    if (true_serie.itemSize() != false_serie.itemSize()) {
        throw std::invalid_argument(
            "True and false series must have same itemSize");
    }
    if (condition.count() != true_serie.count() ||
        condition.count() != false_serie.count()) {
        throw std::invalid_argument("All series must have same count");
    }

    GenSerie<T> result(true_serie.itemSize(), true_serie.count());

    if (true_serie.itemSize() == 1) {
        // Scalar series case
        for (uint32_t i = 0; i < condition.count(); ++i) {
            result.setValue(i, condition.value(i) != 0 ? true_serie.value(i)
                                                       : false_serie.value(i));
        }
    } else {
        // Vector series case
        for (uint32_t i = 0; i < condition.count(); ++i) {
            result.setArray(i, condition.value(i) != 0 ? true_serie.array(i)
                                                       : false_serie.array(i));
        }
    }

    return result;
}

/**
 * @brief Variant that accepts a constant value for true_serie
 */
template <typename T>
GenSerie<T> if_fn(const GenSerie<T> &condition, T true_value,
                  const GenSerie<T> &false_serie) {
    std::vector<T> true_values(false_serie.itemSize(), true_value);
    GenSerie<T> true_serie(false_serie.itemSize(), condition.count());
    for (uint32_t i = 0; i < condition.count(); ++i) {
        true_serie.setArray(i, true_values);
    }
    return if_fn(condition, true_serie, false_serie);
}

/**
 * @brief Variant that accepts a constant value for false_serie
 */
template <typename T>
GenSerie<T> if_fn(const GenSerie<T> &condition, const GenSerie<T> &true_serie,
                  T false_value) {
    std::vector<T> false_values(true_serie.itemSize(), false_value);
    GenSerie<T> false_serie(true_serie.itemSize(), condition.count());
    for (uint32_t i = 0; i < condition.count(); ++i) {
        false_serie.setArray(i, false_values);
    }
    return if_fn(condition, true_serie, false_serie);
}

/**
 * @brief Variant that accepts constant values for both series
 */
template <typename T>
GenSerie<T> if_fn(const GenSerie<T> &condition, T true_value, T false_value) {
    GenSerie<T> result(1, condition.count());
    for (uint32_t i = 0; i < condition.count(); ++i) {
        result.setValue(i, condition.value(i) != 0 ? true_value : false_value);
    }
    return result;
}

// Helper function to create an if operation
template <typename T> auto make_if(T true_value, T false_value) {
    return [=](const GenSerie<T> &condition) {
        return if_fn(condition, true_value, false_value);
    };
}

template <typename T>
auto make_if(const GenSerie<T> &true_serie, const GenSerie<T> &false_serie) {
    return [&](const GenSerie<T> &condition) {
        return if_fn(condition, true_serie, false_serie);
    };
}

} // namespace cond
} // namespace df
