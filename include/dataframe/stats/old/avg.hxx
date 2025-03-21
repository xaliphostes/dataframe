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
 */

namespace df {
namespace stats {

namespace detail {

// // Helper to determine if we're dealing with an array type
// template <typename T> struct is_array : std::false_type {};

// template <typename T, std::size_t N>
// struct is_array<std::array<T, N>> : std::true_type {};

// Mean implementation for scalar types
template <typename T>
inline T compute_avg_impl(
    const Serie<T> &serie,
    typename std::enable_if<!details::is_std_array<T>::value>::type * =
        nullptr) {

    if (serie.empty()) {
        throw std::runtime_error("Cannot compute avg of empty serie");
    }

    T sum = T{};
    for (size_t i = 0; i < serie.size(); ++i) {
        sum += serie[i];
    }
    return sum / static_cast<T>(serie.size());
}

// Mean implementation for array types (vectors, matrices)
template <typename T>
inline T compute_avg_impl(
    const Serie<T> &serie,
    typename std::enable_if<details::is_std_array<T>::value>::type * =
        nullptr) {

    if (serie.empty()) {
        throw std::runtime_error("Cannot compute avg of empty serie");
    }

    T sum{}; // Zero-initialize array
    for (size_t i = 0; i < serie.size(); ++i) {
        for (size_t j = 0; j < details::array_dimension<T>::value; ++j) {
            sum[j] += serie[i][j];
        }
    }

    // Component-wise division
    for (size_t j = 0; j < details::array_dimension<T>::value; ++j) {
        sum[j] /= static_cast<typename T::value_type>(serie.size());
    }

    return sum;
}

} // namespace detail

template <typename T> inline T avg(const Serie<T> &serie) {
    // static_assert(
    //     std::is_arithmetic<typename std::remove_reference<
    //         decltype(std::declval<T>()[0])>::type>::value,
    //     "Serie elements must be arithmetic or arrays of arithmetic types");

    return detail::compute_avg_impl(serie);
}

template <typename T> inline Serie<T> avg_serie(const Serie<T> &serie) {
    return Serie<T>({avg(serie)});
}

template <typename T> inline auto bind_avg() {
    return [](const Serie<T> &serie) { return avg(serie); };
}

} // namespace stats
} // namespace df