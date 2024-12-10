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

#include <dataframe/functional/math/add.h>

namespace df {
namespace math {

Serie add(const std::initializer_list<Serie> &list) {
    Series series(list);

    const Serie &firstSerie = series[0];
    uint32_t itemSize = firstSerie.itemSize();

    for (uint32_t j = 1; j < series.size(); ++j) {
        const Serie &serie = series[j];
        if (firstSerie.count() != serie.count()) {
            throw std::invalid_argument(
                "(add) Series are incompatble. Serie 0 has count=" +
                std::to_string(firstSerie.count()) + " and the Serie " +
                std::to_string(j) +
                " has count=" + std::to_string(serie.count()));
        }
        if (firstSerie.itemSize() != serie.itemSize()) {
            throw std::invalid_argument(
                "(add) Series are incompatble. Serie 0 has itemSize=" +
                std::to_string(firstSerie.itemSize()) + " and the Serie " +
                std::to_string(j) +
                " has itemSize=" + std::to_string(serie.itemSize()));
        }
    }

    return firstSerie.map(
        [series, itemSize](const Array &a, uint32_t i) { // ieme item
            Array r = a;
            for (uint32_t j = 1; j < series.size(); ++j) {
                const Serie &serie = series[j];
                for (uint32_t k = 0; k < itemSize; ++k) {
                    r[k] += serie.value(i)[k];
                }
            }
            return r;
        });
}

Serie add(const Serie &serie, double v) {
    if (serie.itemSize() != 1) {
        throw std::invalid_argument("(add) Serie must have itemSize=1");
    }

    return serie.map([v](double a, uint32_t i) { return a + v; });
}

} // namespace math
} // namespace df
