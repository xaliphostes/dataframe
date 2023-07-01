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

#include <dataframe/operations/math/div.h>

namespace df {

    Serie div(const Serie &serie, double d) {
        
        return serie.map([d](const Array& a, uint32_t i) { // ieme item
            Array r = a;
            for (uint32_t j=0; j<a.size(); ++j) {
                r[j] /= d;
            }
            return r;
        });
    }

    Serie div(const Serie &serie, const Serie& divider) {
        // Checks...
        if (serie.count() != divider.count()) {
            throw std::invalid_argument("(math/div) count of serie (" +
                std::to_string(serie.count()) +
                ") differs from count of divider (" +
                std::to_string(divider.count()) +
                ")");
        }
        if (divider.itemSize() != 1) {
            throw std::invalid_argument("(math/div) itemSize of divider should be 1. Got " +
                std::to_string(divider.itemSize()) );
        }

        return serie.map([divider](const Array& a, uint32_t i) { // ieme item
            double d = divider.scalar(i);
            Array r = a;
            for (uint32_t j=0; j<a.size(); ++j) {
                r[j] /= d;
            }
            return r;
        });
    }

}
