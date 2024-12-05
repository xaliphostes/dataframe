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

#include <dataframe/functional/algebra/det.h>
#include <dataframe/view/Matrix.h>

namespace df {
namespace algebra {

// =====================================================================

Serie det(const Serie &s) {
    if (s.isValid() == false) {
        throw std::invalid_argument("det: serie s is not valid");
    }

    return s.map([s](const Array &v, uint32_t) {
        if ((s.itemSize() == 3 || s.itemSize() == 4) && s.dimension() == 2) {
            Matrix m = v.size() == 3 ? symSquaredMatrix(v) : squaredMatrix(v);
            return Array{m.at(0, 0) * m.at(1, 1) - m.at(0, 1) * m.at(1, 0)};
        }
        if ((s.itemSize() == 6 || s.itemSize() == 9) && s.dimension() == 3) {
            Matrix m = v.size() == 6 ? symSquaredMatrix(v) : squaredMatrix(v);
            return Array{m.at(0, 0) * m.at(1, 1) * m.at(2, 2) -
                         m.at(0, 0) * m.at(1, 2) * m.at(2, 1) -
                         m.at(0, 1) * m.at(1, 0) * m.at(2, 2) +
                         m.at(0, 1) * m.at(1, 2) * m.at(2, 0) +
                         m.at(0, 2) * m.at(1, 0) * m.at(2, 1) -
                         m.at(0, 2) * m.at(1, 1) * m.at(2, 0)};
        }
        throw std::invalid_argument("det: itemSize should be 3, 4, 6 or 9");
    });
}

/*
// See https://www.npmjs.com/package/@rafapaezbas/matrix-determinant in JS:

const cofactor = (array, column) => {
    const subArray = array.filter((e, i) => i !== 0).map(e => e.filter((o, j) =>
j !== column)) return Math.pow(-1, column) * determinant(subArray)
}

const determinant = (array) => {
    if (array.length === 1) return array[0][0]
    let result = 0
    for (let i = 0; i < array.length; i++) {
        result += array[0][i] * cofactor(array, i)
    }
    return result
}
*/

} // namespace algebra
} // namespace df
