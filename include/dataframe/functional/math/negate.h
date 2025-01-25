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

#pragma once
#include <dataframe/Serie.h>

namespace df {
namespace math {

/**
 * @brief Negate values in a serie
 * @param serie Input serie to negate
 * @return GenSerie<T> Negated serie with same dimensions
 */
template<typename T>
GenSerie<T> negate(const GenSerie<T>& serie) {
    GenSerie<T> result(serie.itemSize(), serie.count());
    
    if (serie.itemSize() == 1) {
        // Scalar case
        for (uint32_t i = 0; i < serie.count(); ++i) {
            result.setValue(i, -serie.value(i));
        }
    } else {
        // Vector case
        for (uint32_t i = 0; i < serie.count(); ++i) {
            auto vec = serie.array(i);
            std::vector<T> negated(vec.size());
            for (size_t j = 0; j < vec.size(); ++j) {
                negated[j] = -vec[j];
            }
            result.setArray(i, negated);
        }
    }
    
    return result;
}

MAKE_OP(negate);

} // namespace math
} // namespace df

// --------------------------------------------------

// Operator overload for unary minus
template<typename T>
df::GenSerie<T> operator-(const df::GenSerie<T>& serie) {
    return df::math::negate(serie);
}