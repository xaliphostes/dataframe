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
namespace utils {

/**
 * @example
 * ```cpp
 *  Serie stress(6, {...});
    auto [compressive, tensile] = partition(
        [](const Array &s, uint32_t) {
            return s[0] < 0; // Separate compressive from tensile states
        },
        stress
    );
 * ```
 * @ingroup Utils
 */
template <typename F> auto partition(F &&predicate, const Serie &serie) {
    if constexpr (details::is_scalar_callback_v<F>) {
        // Scalar partition
        if (serie.itemSize() != 1) {
            throw std::invalid_argument(
                "Cannot use scalar callback for Serie with itemSize > 1");
        }

        std::vector<uint32_t> matched_indices, unmatched_indices;
        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (predicate(serie.template get<double>(i), i)) {
                matched_indices.push_back(i);
            } else {
                unmatched_indices.push_back(i);
            }
        }

        Serie matched(1, matched_indices.size());
        Serie unmatched(1, unmatched_indices.size());

        for (uint32_t i = 0; i < matched_indices.size(); ++i) {
            matched.set(i, serie.template get<double>(matched_indices[i]));
        }
        for (uint32_t i = 0; i < unmatched_indices.size(); ++i) {
            unmatched.set(i, serie.template get<double>(unmatched_indices[i]));
        }

        return std::make_tuple(matched, unmatched);
    } else {
        // Vector partition
        std::vector<uint32_t> matched_indices, unmatched_indices;
        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (predicate(serie.template get<Array>(i), i)) {
                matched_indices.push_back(i);
            } else {
                unmatched_indices.push_back(i);
            }
        }

        Serie matched(serie.itemSize(), matched_indices.size());
        Serie unmatched(serie.itemSize(), unmatched_indices.size());

        for (uint32_t i = 0; i < matched_indices.size(); ++i) {
            matched.set(i, serie.template get<Array>(matched_indices[i]));
        }
        for (uint32_t i = 0; i < unmatched_indices.size(); ++i) {
            unmatched.set(i, serie.template get<Array>(unmatched_indices[i]));
        }

        return std::make_tuple(matched, unmatched);
    }
}

} // namespace utils
} // namespace df
