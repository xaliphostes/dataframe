#include "weightedSum.h"

namespace df {

    Serie weigthedSum(const std::initializer_list<Serie> &list, const Array &weights) {
        Series series(list);

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

    Serie weigthedSum(const std::initializer_list<Serie> &list, const std::initializer_list<double> &weights) {
        return weigthedSum(list, Array(weights));
    }

}
