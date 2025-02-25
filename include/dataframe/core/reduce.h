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
#include <dataframe/utils/meta.h>

namespace df {

// -------------------------------------------

namespace details {

template <typename T> struct reduce_result {
    using type = typename std::conditional<is_simple_type<T>::value, T,
                                           Serie<T>>::type;
};

} // namespace details

// -------------------------------------------

template <typename F, typename T, typename AccT>
auto reduce(F &&callback, const Serie<T> &serie, AccT initial) ->
    typename details::reduce_result<AccT>::type {
    AccT result = initial;

    for (size_t i = 0; i < serie.size(); ++i) {
        result = callback(result, serie[i], i);
    }

    if constexpr (details::is_simple_type<AccT>::value) {
        return result;
    } else {
        return Serie<AccT>({result});
    }
}

// Multi series version
template <typename F, typename T, typename AccT, typename... Args>
auto reduce(F &&callback, const Serie<T> &first, const Serie<T> &second,
            AccT initial, const Args &...args) ->
    typename details::reduce_result<AccT>::type {
    AccT result = initial;

    for (size_t i = 0; i < first.size(); ++i) {
        result = callback(result, first[i], second[i], (args[i])..., i);
    }

    if constexpr (details::is_simple_type<AccT>::value) {
        return result;
    } else {
        return Serie<AccT>({result});
    }
}

template <typename F, typename AccT>
auto bind_reduce(F &&callback, AccT initial) {
    return [callback = std::forward<F>(callback), initial](const auto &serie) {
        return reduce(callback, serie, initial);
    };
}

} // namespace df