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

#include <dataframe/operations/algebra/dot.h>

namespace df {

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

    Serie dot(const Serie &a, const Array &b) {
        if (a.value(0).size() != b.size()) {
            return Serie();
        }

        uint32_t itemSize = a.itemSize();

        return a.map([itemSize, b](const Array& arr, uint32_t i) { // ieme item
            Array r = createArray(1, 0) ;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[0] += arr[k] * b[k];
            }
            return r;
        });
    }

}
