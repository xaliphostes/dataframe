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
#include <type_traits>
#include <vector>

namespace df {
namespace details {

// -------------------------------------------------

/**
 * @brief Type trait to check if a type is a simple type (arithmetic, enum, ptr,
 * null...)
 */
template <typename T>
struct is_simple_type
    : std::integral_constant<bool, std::is_arithmetic<T>::value ||
                                       std::is_enum<T>::value ||
                                       std::is_pointer<T>::value ||
                                       std::is_null_pointer<T>::value> {};

/**
 * @brief Type trait to check if a type is a scalar (arithmetic type)
 */
template <typename T> using is_scalar = std::is_arithmetic<T>;

// -------------------------------------------------

/**
 * @brief Type trait to detect if a type is a std::array
 */
template <typename T> struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// Type trait for getting array element type
template <typename T> struct array_element {
    using type = T;
};
template <typename T, std::size_t N> struct array_element<std::array<T, N>> {
    using type = T;
};

// -------------------------------------------------

/**
 * @brief Type trait to detect if a type is a std::vector
 */
template <typename T> struct is_std_vector : std::false_type {};
template <typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

// -------------------------------------------------

/**
 * @brief Type trait to detect if a type is a container-like type (has
 * operator[], size() and value_type)
 */
template <typename T, typename = void> struct is_container : std::false_type {};
template <typename T>
struct is_container<
    T, std::void_t<decltype(std::declval<T>()[0]),
                   decltype(std::declval<T>().size()), typename T::value_type>>
    : std::true_type {};

/**
 * @brief Helper to get element type of a container
 */
template <typename T, typename = void> struct container_value_type {
    using type = T; // fallback for non-containers
};
template <typename T>
struct container_value_type<T, std::void_t<typename T::value_type>> {
    using type = typename T::value_type;
};

/**
 * @brief Type trait for resizable containers (has push_back)
 */
template <typename T, typename = void>
struct is_resizable_container : std::false_type {};

template <typename T>
struct is_resizable_container<T,
                              std::void_t<decltype(std::declval<T>().push_back(
                                  std::declval<typename T::value_type>()))>>
    : std::true_type {};

/**
 * @brief Helper to get size of std::array or 0 for dynamic containers
 */
template <typename T>
struct container_size : std::integral_constant<std::size_t, 0> {};
template <typename T, std::size_t N>
struct container_size<std::array<T, N>>
    : std::integral_constant<std::size_t, N> {};

// -------------------------------------------------

/**
 * @brief Type trait to check if a type has reserve method
 */
template <typename T, typename = void> struct has_reserve : std::false_type {};

template <typename T>
struct has_reserve<
    T, std::void_t<decltype(std::declval<T>().reserve(std::size_t{}))>>
    : std::true_type {};

// -------------------------------------------------

/**
 * @brief Type trait to detect if T has a default constructor
 */
template <typename T>
using is_default_constructible = std::is_default_constructible<T>;

/**
 * @brief Check default constructibility at compile time
 */
template <typename T> void check_default_constructible() {
    static_assert(is_default_constructible<T>::value,
                  "Type must be default constructible. Please add a default "
                  "constructor.");
}

// -------------------------------------------------

/**
 * @brief Helper to get tuple element types
 */
template <typename Tuple> struct tuple_element_types;

template <typename... Types> struct tuple_element_types<std::tuple<Types...>> {
    template <std::size_t I>
    using type = typename std::tuple_element<I, std::tuple<Types...>>::type;
};

// -------------------------------------------------

} // namespace details
} // namespace df
