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
#include <dataframe/functional/common.h>
#include <dataframe/functional/macros.h>

#include <type_traits>

namespace df {
namespace details {

    // Helper pour vérifier les itemSize == 1
    template<typename T, typename... Args>
    inline bool check_item_size_one(const GenSerie<T>& first, const Args&... args) {
        return first.itemSize() == 1 && (... && (args.itemSize() == 1));
    }

    template<typename F, typename T>
    inline constexpr bool is_array_callback_v = is_array_callback<F, T>::value;
}

// Single série - version scalaire
template<typename F, typename T>
auto forEach(F&& callback, const GenSerie<T>& serie) 
    -> std::enable_if_t<!details::accepts_vector<F, T>::value, void>
{
    if (serie.itemSize() != 1) {
        throw std::invalid_argument("Scalar callback can only be used with Serie of itemSize 1");
    }

    for (uint32_t i = 0; i < serie.count(); ++i) {
        callback(serie.value(i), i);
    }
}

// Single série - version array
template<typename F, typename T>
auto forEach(F&& callback, const GenSerie<T>& serie) 
    -> std::enable_if_t<details::accepts_vector<F, T>::value, void>
{
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (serie.itemSize() > 1) {
            callback(serie.array(i), i);
        } else {
            callback(std::vector<T>{serie.value(i)}, i);
        }
    }
}

// Multi séries - version scalaire
template<typename F, typename T, typename... Args>
auto forEach(F&& callback, const GenSerie<T>& first, const GenSerie<T>& second, const Args&... args) 
    -> std::enable_if_t<!details::is_array_callback<F, T, T, decltype(args.value(0))...>::value, void>
{
    if (!details::check_item_size_one(first, second, args...)) {
        throw std::invalid_argument("Scalar callback can only be used with Series of itemSize 1");
    }

    if (!details::check_counts(first, second, args...)) {
        throw std::invalid_argument("All series must have the same count");
    }

    for (uint32_t i = 0; i < first.count(); ++i) {
        callback(first.value(i), second.value(i), args.value(i)..., i);
    }
}

// Multi séries - version array
template<typename F, typename T, typename... Args>
auto forEach(F&& callback, const GenSerie<T>& first, const GenSerie<T>& second, const Args&... args) 
    -> std::enable_if_t<details::is_array_callback<F, T, T, decltype(args.value(0))...>::value, void>
{
    if (!details::check_counts(first, second, args...)) {
        throw std::invalid_argument("All series must have the same count");
    }

    for (uint32_t i = 0; i < first.count(); ++i) {
        callback(
            first.itemSize() > 1 ? first.array(i) : std::vector<T>{first.value(i)},
            second.itemSize() > 1 ? second.array(i) : std::vector<T>{second.value(i)},
            (args.itemSize() > 1 ? args.array(i) : std::vector<T>{args.value(i)})...,
            i
        );
    }
}

MAKE_OP(forEach);

} // namespace df