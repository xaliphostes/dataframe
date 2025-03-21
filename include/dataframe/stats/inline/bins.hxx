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

template <typename T> std::pair<T, T> get_range(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot compute range of empty serie");
    }

    T min_val = serie[0];
    T max_val = serie[0];

    for (size_t i = 1; i < serie.size(); ++i) {
        min_val = std::min(min_val, serie[i]);
        max_val = std::max(max_val, serie[i]);
    }

    // Add small offset if min == max to avoid division by zero
    if (min_val == max_val) {
        max_val += std::numeric_limits<T>::epsilon() * std::abs(max_val) * 100;
    }

    return {min_val, max_val};
}

template <typename T>
size_t get_bin_index(T value, T min_val, T max_val, size_t num_bins) {
    if (value < min_val)
        return 0;
    if (value >= max_val)
        return num_bins - 1;

    const T bin_width = (max_val - min_val) / num_bins;
    return static_cast<size_t>((value - min_val) / bin_width);
}

} // namespace detail

template <typename T>
inline Serie<size_t> bins(const Serie<T> &serie, uint nb) {
    static_assert(std::is_arithmetic<T>::value,
                  "Binning only works with arithmetic types");

    if (nb == 0) {
        throw std::invalid_argument("Number of bins must be greater than 0");
    }

    auto [min_val, max_val] = detail::get_range(serie);
    return bins(serie, nb, min_val, max_val);
}

template <typename T>
Serie<size_t> inline bins(const Serie<T> &serie, uint nb, T min, T max) {
    static_assert(std::is_arithmetic<T>::value,
                  "Binning only works with arithmetic types");

    if (nb == 0) {
        throw std::invalid_argument("Number of bins must be greater than 0");
    }
    if (min >= max) {
        throw std::invalid_argument("min must be less than max");
    }

    // Initialize bins
    std::vector<size_t> counts(nb, 0);

    // Count values in each bin
    for (size_t i = 0; i < serie.size(); ++i) {
        size_t bin_idx = detail::get_bin_index(serie[i], min, max, nb);
        counts[bin_idx]++;
    }

    return Serie<size_t>(counts);
}

// Enable pipe operation
template <typename T> inline auto bind_bins(uint nb) {
    return [nb](const Serie<T> &serie) { return bins(serie, nb); };
}

template <typename T> inline auto bind_bins(uint nb, T min, T max) {
    return [nb, min, max](const Serie<T> &serie) {
        return bins(serie, nb, min, max);
    };
}

} // namespace stats
} // namespace df