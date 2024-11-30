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

#include <cmath>
#include <dataframe/functional/math/minMax.h>
#include <dataframe/functional/math/normalize.h>

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
        // auto minmax = df::minMax(serie);
        auto [min, max] = minMax(serie);

        double l = 1.0 / (max[0] - min[0]);
        return serie.map([l, min](double v, uint32_t) { return l * (v - min[0]); });
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
