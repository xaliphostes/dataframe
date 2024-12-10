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
#include "../Serie.h"

namespace df {
namespace cond {

/**
 * @brief Conditional operation on Serie elements
 * @param serie Input Serie
 * @param check_fn Predicate function
 * @param true_fn Function called when check returns true
 * @param false_fn Function called when check returns false
 * @return Serie
 *
 * @example
 * ```cpp
 * auto result = if_then_else(s,
 *     [](const auto& v) { return v[0] < 0; },
 *     [](const auto& v, uint32_t, const Serie&) {
 *         auto r = v;
 *         r[2] = 1;
 *         return r;
 *     },
 *     [](const auto& v, uint32_t, const Serie&) {
 *         auto r = v;
 *         r[2] = -1;
 *         return r;
 *     }
 * );
 * ```
 */
template <typename Check, typename True, typename False>
Serie if_then_else(const Serie &serie, Check &&check_fn, True &&true_fn,
                   False &&false_fn) {
    return map(serie, [check_fn = std::forward<Check>(check_fn),
                       true_fn = std::forward<True>(true_fn),
                       false_fn = std::forward<False>(false_fn),
                       &serie](const auto &value, uint32_t i) {
        return check_fn(value) ? true_fn(value, i, serie)
                               : false_fn(value, i, serie);
    });
}

MAKE_OP(if_then_else);

} // namespace cond
} // namespace df
