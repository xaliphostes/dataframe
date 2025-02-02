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
#include <algorithm>
#include <dataframe/Serie.h>
#include <dataframe/utils.h>
#include <type_traits>
#include <vector>

namespace df {

template <typename T> inline T quantile(const Serie<T> &serie, double p) {
    static_assert(std::is_arithmetic<T>::value,
                  "Quantile only works with arithmetic types");

    if (serie.empty()) {
        throw std::runtime_error("Cannot compute quantile of empty serie");
    }
    if (p < 0.0 || p > 1.0) {
        throw std::invalid_argument("Quantile value must be between 0 and 1");
    }

    std::vector<T> sorted = serie.asArray();
    std::sort(sorted.begin(), sorted.end());

    if (p == 0.0)
        return sorted.front();
    if (p == 1.0)
        return sorted.back();

    const double h = (sorted.size() - 1) * p;
    const size_t i = static_cast<size_t>(h);
    const double remainder = h - i;

    if (i + 1 >= sorted.size())
        return sorted.back();

    return static_cast<T>(sorted[i] * (1.0 - remainder) +
                          sorted[i + 1] * remainder);
}

template <typename T> inline T q25(const Serie<T> &serie) {
    return quantile(serie, 0.25);
}

template <typename T> inline T q50(const Serie<T> &serie) {
    return quantile(serie, 0.50);
}

template <typename T> inline T q75(const Serie<T> &serie) {
    return quantile(serie, 0.75);
}

template <typename T> inline T iqr(const Serie<T> &serie) {
    return q75(serie) - q25(serie);
}

template <typename T> inline Serie<bool> isOutlier(const Serie<T> &serie) {
    const T q1 = q25(serie);
    const T q3 = q75(serie);
    const T iqr_value = q3 - q1;
    const T lower_bound = q1 - T{1.5} * iqr_value;
    const T upper_bound = q3 + T{1.5} * iqr_value;

    return serie.map([lower_bound, upper_bound](const T &value, size_t) {
        return value < lower_bound || value > upper_bound;
    });
}

template <typename T> inline Serie<bool> notOutlier(const Serie<T> &serie) {
    return isOutlier(serie).map(
        [](const bool &value, size_t) { return !value; });
}

template <typename T> inline auto bind_quantile(double p) {
    return [p](const Serie<T> &serie) { return quantile(serie, p); };
}

template <typename T> inline auto bind_q25() {
    return [](const Serie<T> &serie) { return q25(serie); };
}

template <typename T> inline auto bind_q50() {
    return [](const Serie<T> &serie) { return q50(serie); };
}

template <typename T> inline auto bind_q75() {
    return [](const Serie<T> &serie) { return q75(serie); };
}

template <typename T> inline auto bind_iqr() {
    return [](const Serie<T> &serie) { return iqr(serie); };
}

template <typename T> inline auto bind_isOutlier() {
    return [](const Serie<T> &serie) { return isOutlier(serie); };
}

template <typename T> inline auto bind_notOutlier() {
    return [](const Serie<T> &serie) { return notOutlier(serie); };
}

} // namespace df
