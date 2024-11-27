#pragma once
#include "../../Serie.h"
#include "minMax.h"

namespace df {

/**
 * @brief Normalize a Serie. If itemSize > 1, normalize each item independently,
 * otherwise normalize the entire Serie.
 */
Serie normalize(const Serie& serie)
{
    if (!serie.isValid()) {
        throw std::invalid_argument("Serie is invalid");
    }

    if (serie.itemSize() == 1) {
        // Find min and max of the entire Serie
        // double min = std::numeric_limits<double>::max();
        // double max = std::numeric_limits<double>::lowest();

        // for (uint32_t i = 0; i < serie.count(); ++i)
        // {
        //     double v = serie.template get<double>(i);
        //     min = std::min(min, v);
        //     max = std::max(max, v);
        // }
        auto minmax = df::minMax(serie);

        double l = 1.0 / (minmax[1] - minmax[0]);
        return serie.map([l, minmax](double v, uint32_t) { return l * (v - minmax[0]); });
    } else {
        // Normalize each vector independently
        return map(serie, [](const Array& item, uint32_t) {
            // Compute length (L2 norm)
            double length_squared = std::accumulate(
                item.begin(),
                item.end(),
                0.0,
                [](double acc, double v) { return acc + v * v; });
            double l = 1.0 / std::sqrt(length_squared);

            // Normalize vector
            Array result(item.size());
            std::transform(
                item.begin(),
                item.end(),
                result.begin(),
                [l](double v) { return v * l; });
            return result;
        });
    }
}

}