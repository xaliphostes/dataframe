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
#include <dataframe/functional/macros.h>
#include <dataframe/functional/common.h>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace df {
namespace utils {

/**
 * @brief Apply a transformation only to elements that meet a condition
 * @example
 * ```cpp
 * GenSerie<double> data(1, {-2, 1, -3, 4, -5, 6});
 * // Double only negative numbers
 * auto result = map_if(
 *     [](double val, uint32_t) { return val < 0; },  // predicate
 *     [](double val, uint32_t) { return val * 2; },  // transform
 *     data
 * );
 * ```
 * @ingroup Utils
 */
template <typename Predicate, typename Transform, typename T>
df::GenSerie<T> map_if(Predicate&& predicate, Transform&& transform, const df::GenSerie<T>& serie) {
    if constexpr (std::is_invocable_r_v<bool, Predicate, T, uint32_t>) {
        // Scalar version
        if (serie.itemSize() != 1) {
            throw std::invalid_argument(
                "Cannot use scalar callback for Serie with itemSize > 1");
        }

        df::GenSerie<T> result(1, serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            T value = serie.value(i);
            result.setValue(i, predicate(value, i) ? transform(value, i) : value);
        }
        return result;

    } else {
        // Vector version
        df::GenSerie<T> result(serie.itemSize(), serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            auto arr = serie.array(i);
            result.setArray(i, predicate(arr, i) ? transform(arr, i) : arr);
        }
        return result;
    }
}

} // namespace utils
} // namespace df