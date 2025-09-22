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
#include <array>
#include <cmath>
#include <cstddef>
#include <dataframe/Serie.h>
#include <dataframe/algebra/types.h>
#include <dataframe/core/map.h>
#include <stdexcept>

namespace df {

    template <typename T, size_t N>
    using EigenSystem = std::pair<std::array<T, N>, FullMatrix<T, N>>;

    template <typename T, size_t N>
    Serie<std::array<T, N>> eigenValues(const Serie<SymmetricMatrix<T, N>>& serie);

    template <typename T, size_t N>
    Serie<FullMatrix<T, N>> eigenVectors(const Serie<SymmetricMatrix<T, N>>& serie);

    template <typename T, size_t N>
    Serie<EigenSystem<T, N>> eigenSystem(const Serie<SymmetricMatrix<T, N>>& serie);

} // namespace df

#include "inline/eigen.hxx"
