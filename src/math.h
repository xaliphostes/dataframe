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

    Serie weigthedSum(const Series &series, const Array &alpha) {
        // TODO CHECK:
        // 1) alpha.size() == series.size()
        // 2) All series have same count() and itemSize()
        if (series.size() != alpha.size()) {
                throw std::invalid_argument("(weightedSum) Number of series (" +
                    std::to_string(series.size()) +
                    ") differs from numner of weights (" +
                    std::to_string(alpha.size()) +
                    ")");
            }

        const Serie& firstSerie = series[0];
        uint32_t size = alpha.size();
        uint32_t itemSize = series[0].itemSize();

        return firstSerie.map([series, alpha, size, itemSize](const Array &t, uint32_t i) { // Loop items => i
            Array r = createArray(itemSize, 0);
            for (uint32_t j = 0; j < series.size(); ++j) { // Loop series and alpha => j
                const Serie& serie = series[j];
                const Array& sv = serie.value(i);
                double a = alpha[j];
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
