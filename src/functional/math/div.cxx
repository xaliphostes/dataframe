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

#include <dataframe/functional/math/div.h>
#include <dataframe/functional/map.h>

namespace df {

Serie div(double d, const Serie &serie) {

    return map([d](const Array &a, uint32_t i) { // ieme item
        Array r = a;
        for (uint32_t j = 0; j < a.size(); ++j) {
            r[j] /= d;
        }
        return r;
    }, serie);
}

Serie div(const Serie &divider, const Serie &serie) {
    if (!serie.isValid() || !divider.isValid()) {
        throw std::invalid_argument("Serie(s) invalid");
    }

    if (divider.count() != serie.count()) {
        throw std::invalid_argument(
            "divider count (" + std::to_string(divider.count()) +
            ") must match Serie count (" + std::to_string(serie.count()) + ")");
    }

    if (divider.itemSize() != 1) {
        throw std::invalid_argument("divider Serie must have itemSize of 1");
    }

    if (serie.itemSize() == 1) {
        // Scalar case
        return map(
            [&divider](double v, uint32_t i) {
                double w = divider.get<double>(i);
                if (std::abs(w) < 1e-10) {
                    return 0.0; // or throw
                }
                return v / w;
            },
            serie);
    } else {
        // Vector case
        return map(
            [&divider](const Array &v, uint32_t i) {
                double w = divider.get<double>(i);
                if (std::abs(w) < 1e-10) {
                    return Array(v.size(), 0.0); // or throw
                }
                Array result(v.size());
                for (size_t j = 0; j < v.size(); ++j) {
                    result[j] = v[j] / w;
                }
                return result;
            },
            serie);
    }
}

} // namespace df