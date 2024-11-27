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

#include <dataframe/functional/algebra/cross.h>

namespace df {

    Serie cross(const Serie &a, const Serie &b) {
        if (a.isValid() == false) {
            throw std::invalid_argument("cross: serie a is not valid");
        }
        if (b.isValid() == false) {
            throw std::invalid_argument("cross: serie b is not valid");
        }
        if (a.itemSize() != 3) {
            throw std::invalid_argument("cross: serie a must have itemSize=3");
        }
        if (b.itemSize() != 3) {
            throw std::invalid_argument("cross: serie b must have itemSize=3");
        }

        return a.map( [&](const Array& A, uint32_t i) {
            auto B = b.value(i);
            return Array{
                A[1] * B[2] - A[2] * B[1],
                A[2] * B[0] - A[0] * B[2],
                A[0] * B[1] - A[1] * B[0]
            };
        });
    }

}
