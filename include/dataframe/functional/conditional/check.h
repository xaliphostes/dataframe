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

#pragma once
#include "../../Serie.h"

namespace df {
namespace cond {

/**
 * @brief Check condition on each element of a Serie
 */
template <typename F> auto check(const Serie &serie, F &&cb) {
    if constexpr (details::has_scalar_input_v<F>) {
        // Input is scalar
        Array results;
        results.reserve(serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            results.push_back(cb(serie.template get<double>(i), i) ? 1.0 : 0.0);
        }
        return Serie(1, results);
    } else {
        // Input is vector
        Array results;
        results.reserve(serie.count());
        for (uint32_t i = 0; i < serie.count(); ++i) {
            results.push_back(cb(serie.template get<Array>(i), i) ? 1.0 : 0.0);
        }
        return Serie(1, results);
    }
}

/**
 * @brief Creates a reusable check function
 */
MAKE_OP(check);

} // namespace cond
} // namespace df
