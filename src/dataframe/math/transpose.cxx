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

#include <dataframe/math/transpose.h>

namespace df
{

    /**
     * @brief Transpose a matrix. Only rank-2 matrices with dim 2 or 3.
     */
    Serie transpose(const Serie &serie)
    {

        if (serie.itemSize() != 4 && serie.itemSize() != 9) {
            throw std::invalid_argument("(transpose) items size should be 4 or 9 only (for now). Got " +
                std::to_string(serie.itemSize()));
        }

        if (serie.itemSize() == 4) {
            return serie.map([](const Array &item, uint32_t i) {
                return Array {
                    item[0], item[2],
                    item[1], item[3],
                };
            });
        }
        return serie.map([](const Array &item, uint32_t i) {
            return Array {
                item[0], item[3], item[6],
                item[1], item[4], item[7],
                item[2], item[5], item[8]
            };
        });
    }

}
