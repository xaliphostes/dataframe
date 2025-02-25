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
 */

#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/utils/meta.h>
#include <optional>

namespace df {
namespace nan {

enum class FillMethod {
    LINEAR,   // Linear interpolation between valid points
    NEAREST,  // Use nearest valid value
    PREVIOUS, // Use previous valid value
    NEXT,     // Use next valid value
    MEAN      // Use mean of neighboring valid values
};

// Helper function
template <typename T>
Serie<T> interpolate(const Serie<T> &serie,
                     FillMethod method = FillMethod::LINEAR);

} // namespace nan
} // namespace df

#include "inline/interpolation.hxx"