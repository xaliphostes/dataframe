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
// #include <dataframe/Serie.h>
#include <type_traits>

namespace df {
namespace details {

// --------------------------------------------------
// To check if the value is a floating point number
// --------------------------------------------------
template <typename T>
using is_floating =
    std::is_floating_point<std::remove_cv_t<std::remove_reference_t<T>>>;

template <typename T>
inline constexpr bool is_floating_v = is_floating<T>::value;

template <typename T>
using IsFloating = std::enable_if_t<df::details::is_floating_v<T>, T>;

// --------------------------------------------------
// Trait for detecting if a type is an array
// --------------------------------------------------
template <typename Container> struct is_array : std::false_type {};

template <typename T> struct is_array<std::vector<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_array_v = is_array<std::decay_t<T>>::value;

// --------------------------------------------------
// Type trait pour détecter les callbacks scalaires
// --------------------------------------------------
template <typename F, typename T, typename Enable = void>
struct accepts_scalar : std::false_type {};

template <typename F, typename T>
struct accepts_scalar<F, T,
                      std::void_t<decltype(std::declval<F>()(
                          std::declval<T>(), std::declval<uint32_t>()))>>
    : std::true_type {};

template <typename F, typename T>
inline constexpr bool is_scalar_callback_v = accepts_scalar<F, T>::value;

// --------------------------------------------------
// Type trait pour détecter si le callback accepte des vecteurs
// --------------------------------------------------
template <typename F, typename T, typename Enable = void>
struct accepts_vector : std::false_type {};

template <typename F, typename T>
struct accepts_vector<
    F, T,
    std::void_t<decltype(std::declval<F>()(
        std::declval<const std::vector<T> &>(), std::declval<uint32_t>()))>>
    : std::true_type {
    // This will show up in compiler errors
    static_assert(true, "Vector version selected");
};

template <typename F, typename T>
inline constexpr bool accepts_vector_v = accepts_vector<F, T>::value;

// --------------------------------------------------
// Type trait pour détecter un callback array avec plusieurs paramètres
// --------------------------------------------------
template <typename F, typename... Args> struct is_array_callback;

template <typename F> struct is_array_callback<F> : std::true_type {};

template <typename F, typename T, typename... Args>
struct is_array_callback<F, T, Args...> {
    static constexpr bool value =
        accepts_vector<F, T>::value && is_array_callback<F, Args...>::value;
};


} // namespace details
} // namespace df
