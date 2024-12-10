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

#include <dataframe/functional/math/minMax.h>

namespace df {
namespace math {

MinMax minMax(const Serie &serie) {
    if (!serie.isValid()) {
        throw std::invalid_argument("Serie is invalid");
    }

    if (serie.count() == 0) {
        return MinMax{Array(), Array()};
    }

    if (serie.itemSize() == 1) {
        // Scalar case
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();

        for (uint32_t i = 0; i < serie.count(); ++i) {
            double v = serie.template get<double>(i);
            min = std::min(min, v);
            max = std::max(max, v);
        }

        return MinMax{Array{min}, Array{max}};
    } else {
        // Vector case - component-wise min/max
        const Array &first = serie.template get<Array>(0);
        Array min = first;
        Array max = first;

        for (uint32_t i = 1; i < serie.count(); ++i) {
            const Array &v = serie.template get<Array>(i);
            for (size_t j = 0; j < v.size(); ++j) {
                min[j] = std::min(min[j], v[j]);
                max[j] = std::max(max[j], v[j]);
            }
        }

        return MinMax{min, max};
    }
}

} // namespace math
} // namespace df
