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

#include <dataframe/operations/insar.h>
#include <dataframe/operations/dot.h>
#include <cmath>

namespace df {

    /**
     * @brief Compute the insar Serie (itemSize = 1)
     * @param u The displacement vector field
     * @param los The satellite direction (a 3D vector)
     * @return Serie of itemSize=1
     */
    Serie insar(const Serie &u, const Array& los) {
        if (!u.isValid() || u.itemSize() != 3) {
            return Serie();
        }

        if (los.size() != 3) {
            return Serie();
        }

        return dot(u, los);
    }

    static inline double frac(double val) {
        return val - std::floor(val);
    }

    /**
     * @brief Compute the fringes given the insar Serie
     * @param insar The insar computed from {@link insar}
     * @param fringeSpacing The spacing of teh fringes
     * @return Serie of itemSize=1
     */
    Serie fringes(const Serie &insar, double fringeSpacing) {
        if (!insar.isValid() || insar.itemSize() != 1) {
            return Serie();
        }

        return insar.map( [fringeSpacing](const Array &v, uint32_t) {
            return Array{std::fabs(fringeSpacing * frac(v[0] / fringeSpacing))};
        });
    }

}
