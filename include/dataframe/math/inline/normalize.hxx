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

namespace df {

namespace detail {

// Helper to determine if a type is an std::array
template <typename T> struct is_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// Get min/max for scalar types
template <typename T>
inline std::pair<T, T>
get_bounds(const Serie<T> &serie,
           typename std::enable_if<!is_std_array<T>::value>::type * = nullptr) {

    if (serie.empty()) {
        return {T{}, T{}};
    }

    T min_val = serie[0];
    T max_val = serie[0];

    serie.forEach([&](const T &val, size_t) {
        min_val = std::min(min_val, val);
        max_val = std::max(max_val, val);
    });

    return {min_val, max_val};
}

// Get min/max for array types (e.g., Position)
template <typename T>
inline std::pair<typename component_type<T>::type,
                 typename component_type<T>::type>
get_bounds(const Serie<T> &serie,
           typename std::enable_if<is_std_array<T>::value>::type * = nullptr) {

    using ComponentType = typename component_type<T>::type;
    if (serie.empty()) {
        return {ComponentType{}, ComponentType{}};
    }

    ComponentType min_val = serie[0][0];
    ComponentType max_val = serie[0][0];

    serie.forEach([&](const T &arr, size_t) {
        for (const auto &val : arr) {
            min_val = std::min(min_val, val);
            max_val = std::max(max_val, val);
        }
    });

    return {min_val, max_val};
}

// Normalize scalar values
template <typename T>
inline T normalize_value(const T &val, T min_val, T max_val, T target_min,
                         T target_max) {
    if (std::abs(max_val - min_val) < std::numeric_limits<T>::epsilon()) {
        return target_min; // Handle zero-range case
    }
    return target_min +
           (val - min_val) * (target_max - target_min) / (max_val - min_val);
}

} // namespace detail

template <typename T>
NormalizeConfig<T>::NormalizeConfig(T min, T max)
    : target_min(min), target_max(max) {}

template <typename T>
inline Serie<T> normalize(
    const NormalizeConfig<typename detail::component_type<T>::type> &config,
    const Serie<T> &serie) {

    using ComponentType = typename detail::component_type<T>::type;
    auto [min_val, max_val] = detail::get_bounds(serie);

    if constexpr (detail::is_std_array<T>::value) {
        // Handle array types (e.g., Position)
        return serie.map([&](const T &arr, size_t) {
            T result;
            for (size_t i = 0; i < arr.size(); ++i) {
                result[i] = detail::normalize_value(arr[i], min_val, max_val,
                                                    config.target_min,
                                                    config.target_max);
            }
            return result;
        });
    } else {
        // Handle scalar types
        return serie.map([&](const T &val, size_t) {
            return detail::normalize_value(
                val, min_val, max_val, config.target_min, config.target_max);
        });
    }
}

template <typename T> inline Serie<T> normalize(const Serie<T> &serie) {
    using ComponentType = typename detail::component_type<T>::type;
    return normalize(NormalizeConfig<ComponentType>{}, serie);
}

template <typename T>
inline Serie<T> normalize(const Serie<T> &serie,
                          typename detail::component_type<T>::type target_min,
                          typename detail::component_type<T>::type target_max) {

    using ComponentType = typename detail::component_type<T>::type;
    return normalize(NormalizeConfig<ComponentType>{target_min, target_max},
                     serie);
}

} // namespace df
