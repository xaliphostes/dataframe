/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include <dataframe/utils/utils.h>
#include <type_traits>

namespace df {

// Single series version
template <typename F, typename T>
inline auto filter(F &&predicate, const Serie<T> &serie) -> Serie<T> {
    std::vector<T> filtered;
    filtered.reserve(serie.size()); // Reserve max possible size

    for (size_t i = 0; i < serie.size(); ++i) {
        if (predicate(serie[i], i)) {
            filtered.push_back(serie[i]);
        }
    }

    return Serie<T>(filtered);
}

// Multi series version
template <typename F, typename T, typename... Args>
inline auto filter(F &&predicate, const Serie<T> &first, const Serie<T> &second,
            const Args &...args) -> Serie<T> {
    std::vector<T> filtered;
    filtered.reserve(first.size()); // Reserve max possible size

    for (size_t i = 0; i < first.size(); ++i) {
        if (predicate(first[i], second[i], (args[i])..., i)) {
            filtered.push_back(first[i]);
        }
    }

    return Serie<T>(filtered);
}

// Bind function for multiple series
template <typename F> inline auto bind_filter(F &&predicate, const auto &second) {
    return [pred = std::forward<F>(predicate), &second](const auto &first) {
        return filter(pred, first, second);
    };
}

} // namespace df
