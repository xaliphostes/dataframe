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

#include <dataframe/math/scale.h>

namespace df {

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

}
