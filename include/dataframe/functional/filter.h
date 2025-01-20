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
#include <dataframe/functional/common.h>
#include <dataframe/functional/macros.h>
#include <type_traits>

namespace df {

// Version scalaire (itemSize=1)
template <typename F, typename T>
auto filter(F &&predicate, const GenSerie<T> &serie)
    -> std::enable_if_t<details::accepts_scalar<F, T>::value, GenSerie<T>> {
    if (serie.itemSize() != 1) {
        throw std::invalid_argument(
            "Scalar predicate can only be used with Serie of itemSize 1");
    }

    std::vector<T> filtered;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (predicate(serie.value(i), i)) {
            filtered.push_back(serie.value(i));
        }
    }

    return GenSerie<T>(1, filtered);
}

// Version array (itemSize>1)
template <typename F, typename T>
auto filter(F &&predicate, const GenSerie<T> &serie)
    -> std::enable_if_t<details::accepts_vector<F, T>::value, GenSerie<T>> {
    std::vector<T> filtered;
    filtered.reserve(serie.size());

    for (uint32_t i = 0; i < serie.count(); ++i) {
        auto values = serie.array(i);
        if (predicate(values, i)) {
            filtered.insert(filtered.end(), values.begin(), values.end());
        }
    }

    return GenSerie<T>(serie.itemSize(), filtered);
}

MAKE_OP(filter);

} // namespace df