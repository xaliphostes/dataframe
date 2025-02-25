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
 */

#pragma once
#include <array>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <limits>
#include <type_traits>

namespace df {

namespace detail {
template <typename T> struct component_type {
    using type = T;
};
template <typename T, std::size_t N> struct component_type<std::array<T, N>> {
    using type = T;
};
} // namespace detail

/**
 * Configuration for normalization
 */
template <typename T> struct NormalizeConfig {
    T target_min;
    T target_max;
    NormalizeConfig(T min = T{0}, T max = T{1});
};

/**
 * Normalize a serie to a target range (default [0,1])
 */
template <typename T>
Serie<T> normalize(
    const NormalizeConfig<typename detail::component_type<T>::type> &config,
    const Serie<T> &serie);

/**
 * Convenience function for default [0,1] normalization
 */
template <typename T> Serie<T> normalize(const Serie<T> &serie);

/**
 * Convenience function for custom range normalization
 */
template <typename T>
Serie<T> normalize(const Serie<T> &serie,
                   typename detail::component_type<T>::type target_min,
                   typename detail::component_type<T>::type target_max);

// Enable pipe operation
MAKE_OP(normalize)

} // namespace df

#include "inline/normalize.hxx"