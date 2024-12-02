/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once
#include <array>
#include <dataframe/functional/filter.h>
#include <functional>

namespace df {
// /**
//  * @brief Rejects elements from Series based on a predicate, i.e., **remove
//  * elements matching predicate** This is the opposite of filter.
//  */
// template <typename F, typename... Args>
// auto reject(F &&predicate, const Args &...args) {
//     return filter(std::not_fn(std::forward<F>(predicate)), args...);
// }
// /**
//  * @brief Creates a reusable reject function
//  */
// template <typename F> auto make_reject(F &&cb) {
//     static_assert(details::callback_filter_traits<F>::returns_bool,
//                   "Reject predicate must return bool");
//     return [cb = std::forward<F>(cb)](const auto &serie) {
//         return reject(cb, serie);
//     };
// }
} // namespace df

namespace df {

namespace details {
// Same as in filter.h...
}

template <typename F>
auto __reject__(F &&cb, const Serie &serie)
    -> std::enable_if_t<details::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (!cb(serie.template get<double>(i), i)) {
            indices.push_back(i);
        }
    }

    Serie result(1, indices.size());
    for (uint32_t i = 0; i < indices.size(); ++i) {
        result.template set(i, serie.template get<double>(indices[i]));
    }
    return result;
}

template <typename F>
auto __reject__(F &&cb, const Serie &serie)
    -> std::enable_if_t<!details::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (!cb(serie.template get<Array>(i), i)) {
            indices.push_back(i);
        }
    }

    Serie result(serie.itemSize(), indices.size());
    for (uint32_t i = 0; i < indices.size(); ++i) {
        result.template set(i, serie.template get<Array>(indices[i]));
    }
    return result;
}


template <typename F, typename... Args>
auto reject(F &&predicate, const Args &...args) {
    if constexpr (details::is_multi_series_call<Args...>::value) {
        // Multiple Series case
        static_assert(std::conjunction<details::is_serie<Args>...>::value,
                      "All arguments after predicate must be Series");

        // Check counts match
        std::array<size_t, sizeof...(args)> counts = {
            details::get_count(args)...};
        for (size_t i = 1; i < counts.size(); ++i) {
            if (counts[i] != counts[0]) {
                throw std::invalid_argument(
                    "All Series must have the same count");
            }
        }

        // Collect indices that satisfy the predicate
        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < counts[0]; ++i) {
            if (predicate(args.template get<Array>(i)..., i) == false) {
                indices.push_back(i);
            }
        }

        // Create filtered Series
        auto filter_one = [&indices](const Serie &s) {
            Serie filtered(s.itemSize(), indices.size());
            for (uint32_t i = 0; i < indices.size(); ++i) {
                filtered.set(i, s.template get<Array>(indices[i]));
            }
            return filtered;
        };

        return std::make_tuple(filter_one(args)...);
    } else {
        // Single Serie case (original filter implementation)
        static_assert(sizeof...(args) == 1,
                      "Single Serie reject requires exactly one Serie");
        const auto &serie = std::get<0>(std::forward_as_tuple(args...));
        // ... rest of original implementation ...
        return __reject__(predicate, serie);
    }
}

template <typename F> auto make_reject(F &&cb) {
    return [cb = std::forward<F>(cb)](const auto &...args) {
        return reject(cb, args...);
    };
}

// ----------------------------------------

template <typename Pred, typename... TheSeries>
Series rejectAll(Pred &&predicate, const TheSeries &...series) {
    static_assert(details::all_are_series<TheSeries...>::value,
                  "All arguments after predicate must be Series");
    static_assert(sizeof...(series) >= 2,
                  "rejectAll requires at least 2 Series");

    // Check all Series have the same count
    std::array<size_t, sizeof...(series)> counts = {
        details::get_count(series)...};
    for (size_t i = 1; i < counts.size(); ++i) {
        if (counts[i] != counts[0]) {
            throw std::invalid_argument(
                "All Series must have the same count. First is " +
                std::to_string(counts[0]) + " and " + std::to_string(i) +
                "eme is " + std::to_string(counts[i]));
        }
    }

    // Collect indices that satisfy the predicate
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < counts[0]; ++i) {
        if (!predicate(series.template get<Array>(i)...)) {
            indices.push_back(i);
        }
    }

    // Create filtered Series
    auto reject_one = [&indices](const Serie &s) {
        Serie filtered(s.itemSize(), indices.size());
        for (uint32_t i = 0; i < indices.size(); ++i) {
            filtered.set(i, s.template get<Array>(indices[i]));
        }
        return filtered;
    };

    return Series{reject_one(series)...};
}

/**
 * @brief Creates a reusable filter function
 */
template <typename F> auto make_rejectAll(F &&cb) {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Reject predicate must return bool");

    return [cb = std::forward<F>(cb)](const auto &serie) {
        return rejectAll(cb, serie);
    };
}

} // namespace df