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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/utils.h>
#include <type_traits>

namespace df {

namespace detail {

template <typename T> T compute_mean(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot compute mean of empty serie");
    }
    T sum = T{};
    for (size_t i = 0; i < serie.size(); ++i) {
        sum += serie[i];
    }
    return sum / static_cast<T>(serie.size());
}

} // namespace detail



template <typename T>
inline T covariance(const Serie<T> &serie1, const Serie<T> &serie2) {
    static_assert(std::is_arithmetic<T>::value,
                  "Covariance only works with arithmetic types");

    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same size for covariance");
    }
    if (serie1.empty()) {
        throw std::runtime_error("Cannot compute covariance of empty series");
    }

    // Compute means
    const T mean1 = detail::compute_mean(serie1);
    const T mean2 = detail::compute_mean(serie2);

    // Compute covariance
    T cov = T{};
    for (size_t i = 0; i < serie1.size(); ++i) {
        cov += (serie1[i] - mean1) * (serie2[i] - mean2);
    }

    // Return population covariance
    return cov / static_cast<T>(serie1.size());
}

template <typename T>
inline T sample_covariance(const Serie<T> &serie1, const Serie<T> &serie2) {
    if (serie1.size() < 2) {
        throw std::runtime_error(
            "Need at least 2 samples for sample covariance");
    }

    // Leverage population covariance computation
    T pop_cov = covariance(serie1, serie2);

    // Adjust for sample covariance
    return pop_cov * static_cast<T>(serie1.size()) /
           static_cast<T>(serie1.size() - 1);
}

// Enable pipe operation
template <typename T> inline auto bind_covariance(const Serie<T> &other) {
    return [&other](const Serie<T> &serie) { return covariance(serie, other); };
}

template <typename T>
inline auto bind_sample_covariance(const Serie<T> &other) {
    return [&other](const Serie<T> &serie) {
        return sample_covariance(serie, other);
    };
}

} // namespace df