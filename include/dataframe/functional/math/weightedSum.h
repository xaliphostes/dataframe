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

namespace df {
namespace math {

template <typename T>
details::IsSerieFloating<T>
weigthedSum(const std::initializer_list<GenSerie<T>> &list,
            const Array<T> &weights) {
    if (list.size() != weights.size()) {
        throw std::invalid_argument(
            "Number of series must match number of weights");
    }

    if (list.size() == 0) {
        return GenSerie<T>();
    }

    const auto &first = *list.begin();
    GenSerie<T> result(first.itemSize(), first.count());

    for (const auto &serie : list) {
        if (serie.count() != first.count() ||
            serie.itemSize() != first.itemSize()) {
            throw std::invalid_argument("All series must have same dimensions");
        }
    }

    auto it_serie = list.begin();
    auto it_weight = weights.begin();

    for (uint32_t i = 0; i < first.count(); ++i) {
        std::vector<T> sum(first.itemSize(), 0);
        it_serie = list.begin();
        it_weight = weights.begin();

        while (it_serie != list.end()) {
            auto arr = it_serie->array(i);
            for (size_t j = 0; j < arr.size(); ++j) {
                sum[j] += arr[j] * static_cast<T>(*it_weight);
            }
            ++it_serie;
            ++it_weight;
        }
        result.setArray(i, sum);
    }
    return result;
}

template <typename T>
details::IsSerieFloating<T>
weigthedSum(const std::initializer_list<GenSerie<T>> &list,
            const std::initializer_list<double> &weights) {
    return weigthedSum(list, Array<T>(weights.begin(), weights.end()));
}

} // namespace math
} // namespace df
