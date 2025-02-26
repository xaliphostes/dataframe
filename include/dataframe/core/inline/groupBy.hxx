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

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace df {

namespace detail {

// Helper to extract keys from a series using a key function
template <typename T, typename KeyFunc>
auto extract_keys(const Serie<T> &serie, KeyFunc &&key_func) {
    using KeyType = std::decay_t<decltype(key_func(std::declval<T>(), 0))>;

    std::vector<KeyType> keys;
    keys.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        keys.push_back(key_func(serie[i], i));
    }

    return keys;
}

// Helper to collect unique keys in the order they first appear
template <typename KeyType>
std::vector<KeyType> collect_unique_keys(const std::vector<KeyType> &keys) {
    std::vector<KeyType> unique_keys;
    std::unordered_set<KeyType> seen;

    for (const auto &key : keys) {
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            unique_keys.push_back(key);
        }
    }

    return unique_keys;
}

// Helper to find indices for each key
template <typename KeyType>
std::unordered_map<KeyType, std::vector<size_t>>
group_indices(const std::vector<KeyType> &keys) {
    std::unordered_map<KeyType, std::vector<size_t>> groups;

    for (size_t i = 0; i < keys.size(); ++i) {
        groups[keys[i]].push_back(i);
    }

    return groups;
}

// Helper to create a series from selected indices
template <typename T>
Serie<T> create_series_from_indices(const Serie<T> &serie,
                                    const std::vector<size_t> &indices) {
    std::vector<T> values;
    values.reserve(indices.size());

    for (size_t idx : indices) {
        values.push_back(serie[idx]);
    }

    return Serie<T>(values);
}

} // namespace detail

// ----------------------------------------------------------

/**
 * Group a single series by a key function
 *
 * The key function should take a value and an index and return a key
 * Returns a map where keys are the result of the key function, and
 * values are series containing the matching elements
 */
template <typename T, typename KeyFunc>
inline auto groupBy(const Serie<T> &serie, KeyFunc &&key_func) {
    using KeyType = std::decay_t<decltype(key_func(std::declval<T>(), 0))>;

    // Extract keys for each element
    auto keys = detail::extract_keys(serie, key_func);

    // Get unique keys in order of first appearance
    auto unique_keys = detail::collect_unique_keys(keys);

    // Group indices by key
    auto grouped_indices = detail::group_indices(keys);

    // Create result map preserving first-appearance order of keys
    std::map<KeyType, Serie<T>> result;

    for (const auto &key : unique_keys) {
        const auto &indices = grouped_indices[key];
        result[key] = detail::create_series_from_indices(serie, indices);
    }

    return result;
}

/**
 * Group multiple series by a key function applied to the first series
 *
 * The key function should take a value from the first series and an index, and
 * return a key Returns a map where keys are the result of the key function, and
 * values are tuples of series containing the matching elements
 */
template <typename T, typename KeyFunc, typename... Ts>
inline auto groupBy(const Serie<T> &first, KeyFunc &&key_func,
                    const Serie<Ts> &...rest) {
    // Ensure all series have the same size
    const size_t size = first.size();
    if (!((rest.size() == size) && ...)) {
        throw std::runtime_error(
            "All series must have the same size for groupBy operation");
    }

    using KeyType = std::decay_t<decltype(key_func(std::declval<T>(), 0))>;

    // Extract keys for each element based on the first series
    auto keys = detail::extract_keys(first, key_func);

    // Get unique keys in order of first appearance
    auto unique_keys = detail::collect_unique_keys(keys);

    // Group indices by key
    auto grouped_indices = detail::group_indices(keys);

    // Create result map with tuples of series
    std::map<KeyType, std::tuple<Serie<T>, Serie<Ts>...>> result;

    for (const auto &key : unique_keys) {
        const auto &indices = grouped_indices[key];
        result[key] = std::make_tuple(
            detail::create_series_from_indices(first, indices),
            detail::create_series_from_indices(rest, indices)...);
    }

    return result;
}

/**
 * Group by a predicate function
 *
 * The predicate takes a value and returns a boolean. Elements are grouped into
 * two categories: those that satisfy the predicate (true) and those that don't
 * (false).
 */
template <typename T, typename Predicate>
inline auto groupByPredicate(const Serie<T> &serie, Predicate &&pred) {
    return groupBy(serie,
                   [&pred](const T &value, size_t) { return pred(value); });
}

/**
 * Helper to create a bound groupBy operation for use in pipe
 */
template <typename KeyFunc> inline auto bind_groupBy(KeyFunc &&key_func) {
    return [key_func = std::forward<KeyFunc>(key_func)](const auto &serie) {
        return groupBy(serie, key_func);
    };
}

/**
 * Helper to create a bound groupByPredicate operation for use in pipe
 */
template <typename Predicate>
inline auto bind_groupByPredicate(Predicate &&pred) {
    return [pred = std::forward<Predicate>(pred)](const auto &serie) {
        return groupByPredicate(serie, pred);
    };
}

} // namespace df
