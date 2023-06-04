#include "add.h"

namespace df {

    Serie add(const std::initializer_list<Serie> &list) {
        Series series(list) ;

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

}
