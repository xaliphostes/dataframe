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

template <typename F, typename T>
inline Serie<T> reject(F &&predicate, const Serie<T> &serie) {
    std::vector<T> result;
    result.reserve(serie.size()); // Reserve maximum possible size

    for (size_t i = 0; i < serie.size(); ++i) {
        if (!predicate(serie[i], i)) {
            result.push_back(serie[i]);
        }
    }

    return Serie<T>(result);
}

template <typename F, typename T, typename... Args>
inline Serie<T> reject(F &&predicate, const Serie<T> &first,
                       const Serie<T> &second, const Args &...args) {
    if (first.size() != second.size() ||
        (!((first.size() == args.size()) && ...))) {
        throw std::runtime_error("All series must have the same size");
    }

    std::vector<T> result;
    result.reserve(first.size());

    for (size_t i = 0; i < first.size(); ++i) {
        if (!predicate(first[i], second[i], (args[i])..., i)) {
            result.push_back(first[i]);
        }
    }

    return Serie<T>(result);
}

template <typename T> inline auto less_than(T threshold) {
    return [threshold](const T &value, size_t) { return value < threshold; };
}

template <typename T> inline auto greater_than(T threshold) {
    return [threshold](const T &value, size_t) { return value > threshold; };
}

template <typename T> inline auto outside(T min_val, T max_val) {
    return [min_val, max_val](const T &value, size_t) {
        return value < min_val || value > max_val;
    };
}

template <typename T> inline auto equal_to(T value) {
    return [value](const T &val, size_t) { return val == value; };
}

template <typename F> inline auto bind_reject(F &&predicate) {
    return [f = std::forward<F>(predicate)](const auto &serie) {
        return reject(f, serie);
    };
}

} // namespace df