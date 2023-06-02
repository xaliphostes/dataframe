/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include "Serie.h"
#include "utils.h"

namespace df
{

    Serie add(const Series &series) {
        // TODO CHECK:
        // 1) All series have same count() and itemSize()



        const Serie& firstSerie = series[0];
        uint32_t itemSize = firstSerie.itemSize() ;

        for (uint32_t j=1; j<series.size(); ++j) {
            const Serie& serie = series[j];
            if (firstSerie.count() != serie.count()) {
                throw std::invalid_argument("(add) Series are incompatble. Serie 0 has count=" +
                    std::to_string(firstSerie.count()) +
                    " and the Serie " +
                    std::to_string(j) +
                    " has count=" +
                    std::to_string(serie.count()));
            }
            if (firstSerie.itemSize() != serie.itemSize()) {
                throw std::invalid_argument("(add) Series are incompatble. Serie 0 has itemSize=" +
                    std::to_string(firstSerie.itemSize()) +
                    " and the Serie " +
                    std::to_string(j) +
                    " has itemSize=" +
                    std::to_string(serie.itemSize()));
            }
        }

        return firstSerie.map([series, itemSize](const Array& a, uint32_t i) { // ieme item
            Array r = a;
            for (uint32_t j=1; j<series.size(); ++j) {
                const Serie& serie = series[j];
                for (uint32_t k=0; k<itemSize; ++k) {
                    r[k] += serie.value(i)[k];
                }
            }
            return r;
        });
    }

    Serie add(const Serie &a, const Serie& b) {
        const Serie& firstSerie = a;
        uint32_t itemSize = firstSerie.itemSize() ;

            
            if (a.count() != b.count()) {
                throw std::invalid_argument("(add) Series are incompatble. Serie a has count=" +
                    std::to_string(firstSerie.count()) +
                    " and the Serie b has count=" +
                    std::to_string(b.count()));
            }
            if (a.itemSize() != b.itemSize()) {
                throw std::invalid_argument("(add) Series are incompatble. Serie a has itemSize=" +
                    std::to_string(firstSerie.itemSize()) +
                    " and the Serie has itemSize " +
                    std::to_string(b.itemSize()));
            }

        return a.map([b, itemSize](const Array& a, uint32_t i) { // ieme item
            Array r = a;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[k] += b.value(i)[k];
            }
            return r;
        });
    }

    Serie weigthedSum(const Series &series, const Array &weights) {
        // TODO CHECK:
        // 1) alpha.size() == series.size()
        // 2) All series have same count() and itemSize()
        if (series.size() != weights.size()) {
                throw std::invalid_argument("(weightedSum) Number of series (" +
                    std::to_string(series.size()) +
                    ") differs from the number of weights (" +
                    std::to_string(weights.size()) +
                    ")");
            }

        uint32_t count = series[0].count();
        uint32_t itemSize = series[0].itemSize();
        uint32_t index = -1;
        for (const auto &serie: series) {
            index++;
            if (serie.count() != count) {
                throw std::invalid_argument("(weightedSum) Count (" +
                std::to_string(serie.count()) +
                ") of the Serie at index " +
                    std::to_string(index) +
                    " differs from the count of the first Serie (" +
                    std::to_string(count) +
                    ")");
            }
            if (serie.itemSize() != itemSize) {
                throw std::invalid_argument("(weightedSum) itemSize (" +
                std::to_string(serie.itemSize()) +
                ") of the Serie at index " +
                    std::to_string(index) +
                    " differs from the itemSize of the first Serie (" +
                    std::to_string(itemSize) +
                    ")");
            }
        }

        const Serie& firstSerie = series[0];
        uint32_t size = weights.size();

        return firstSerie.map([series, weights, size, itemSize](const Array &t, uint32_t i) { // Loop items => i
            Array r = createArray(itemSize, 0);
            for (uint32_t j = 0; j < series.size(); ++j) { // Loop series and weights => j
                const Serie& serie = series[j];
                const Array& sv = serie.value(i);
                double a = weights[j];
                for (uint32_t k=0; k<itemSize; ++k) {
                    r[k] += sv[k]*a;
                }
            }
            return r;
        });
    }

    Serie scale(const Serie& s, double scale) {
        uint32_t size = s.itemSize();
        return s.map([scale, size](const Array& a, uint32_t i) {
            Array r = a ;
            for (uint32_t k=0; k<size; ++k) {
                r[k] *= scale;
            }
            return r;
        });
    }

    Serie scale(const Serie& s, const Array& scales) {
        if (s.itemSize() != scales.size()) {
            throw std::invalid_argument("(scale) Serie itemSize (" +
                std::to_string(s.itemSize()) +
                " differs from scale size (" +
                std::to_string(scales.size()) +
                ")");
        }

        uint32_t size = s.itemSize();
        return s.map([scales, size](const Array& a, uint32_t i) {
            Array r = a ;
            for (uint32_t k=0; k<size; ++k) {
                r[k] *= scales[k];
            }
            return r;
        });
    }

    Serie negate(const Serie &serie) {
        uint32_t itemSize = serie.itemSize();
        return serie.map([itemSize](const Array& a, uint32_t i) { // ieme item
            Array r = a ;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[k] *= -1;
            }
            return r;
        });
    }

    Serie dot(const Serie &a, const Serie &b) {
        uint32_t itemSize = a.itemSize();
        return a.map([itemSize, b](const Array& arr, uint32_t i) { // ieme item
            Array r = createArray(1, 0) ;
            const Array& bb = b.value(i) ;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[0] += arr[k] * bb[k];
            }
            return r;
        });
    }

    // etc...
}
