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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/common.h>

namespace df {

/**
 * Single serie version
 * @code
 * auto result = df::map_if(
 *     [](double val, size_t) { return val < 0; },  // predicate
 *     [](double val, size_t) { return val * 2; },  // transform
 *     data
 * );
 * @endcode
 *
 * Multi series version
 * @code
 * auto result = df::map_if(
 *     [](double val, bool flag, size_t) { return flag; },  // predicate
 *     [](double val, bool, size_t) { return val * 2; },    // transform
 *     values,
 *     flags
 * );
 * @endcode
 *
 * Pipe Operator Support
 * @code
 * auto result = series | df::bind_map_if(predicate, transform);
 * // or with multiple series
 * auto result = series | df::bind_map_if(predicate, transform, second_series);
 * @encode
 */

// Single series version
template <typename P, typename F, typename T>
auto map_if(P &&predicate, F &&transform,
            const Serie<T> &serie) -> Serie<T> {
    std::vector<T> result;
    result.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        if (predicate(serie[i], i)) {
            result.push_back(transform(serie[i], i));
        } else {
            result.push_back(serie[i]);
        }
    }

    return Serie<T>(result);
}

// Two series version
template <typename P, typename F, typename T, typename U>
auto map_if(P &&predicate, F &&transform, const Serie<T> &first,
            const Serie<U> &second) -> Serie<T> {
    std::vector<T> result;
    result.reserve(first.size());

    for (size_t i = 0; i < first.size(); ++i) {
        if (predicate(first[i], second[i], i)) {
            result.push_back(transform(first[i], second[i], i));
        } else {
            result.push_back(first[i]);
        }
    }

    return Serie<T>(result);
}

// Three series version
template <typename P, typename F, typename T, typename U, typename V>
auto map_if(P &&predicate, F &&transform, const Serie<T> &first,
            const Serie<U> &second,
            const Serie<V> &third) -> Serie<T> {
    std::vector<T> result;
    result.reserve(first.size());

    for (size_t i = 0; i < first.size(); ++i) {
        if (predicate(first[i], second[i], third[i], i)) {
            result.push_back(transform(first[i], second[i], third[i], i));
        } else {
            result.push_back(first[i]);
        }
    }

    return Serie<T>(result);
}

// Bind functions for pipe operator
template <typename P, typename F>
auto bind_map_if(P &&predicate, F &&transform) {
    return [pred = std::forward<P>(predicate),
            trans = std::forward<F>(transform)](const auto &serie) {
        return map_if(pred, trans, serie);
    };
}

// Two series bind
template <typename P, typename F, typename U>
auto bind_map_if(P &&predicate, F &&transform, const Serie<U> &second) {
    return [pred = std::forward<P>(predicate),
            trans = std::forward<F>(transform), &second](const auto &first) {
        return map_if(pred, trans, first, second);
    };
}

// Three series bind
template <typename P, typename F, typename U, typename V>
auto bind_map_if_3(P &&predicate, F &&transform, const Serie<U> &second,
                   const Serie<V> &third) {
    return
        [pred = std::forward<P>(predicate), trans = std::forward<F>(transform),
         &second, &third](const auto &first) {
            return map_if(pred, trans, first, second, third);
        };
}

MAKE_OP(map_if);

} // namespace df
