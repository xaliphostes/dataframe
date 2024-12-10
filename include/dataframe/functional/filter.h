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
#include <dataframe/Serie.h>
#include <dataframe/utils.h>
#include <tuple>

namespace df {

namespace details {

// Base template
template <typename T> struct function_filter_traits;

// Spécialisation pour les lambdas
template <typename C, typename R, typename... Args>
struct function_filter_traits<R (C::*)(Args...) const> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

// Spécialisation pour les fonctions normales
template <typename R, typename... Args>
struct function_filter_traits<R (*)(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

// Spécialisation pour les types de fonction
template <typename R, typename... Args>
struct function_filter_traits<R(Args...)> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

// Pour déduire à partir du type actuel
template <typename F>
struct function_filter_traits
    : function_filter_traits<
          decltype(&std::remove_reference_t<F>::operator())> {};

// --------------------- added later on...

// Type trait pour détecter les callbacks scalaires
template <typename F> struct callback_filter_traits {
    using clean_type = std::remove_cv_t<std::remove_reference_t<F>>;
    using traits = function_filter_traits<clean_type>;
    using first_arg = std::tuple_element_t<0, typename traits::args_tuple>;

    static constexpr bool is_scalar =
        std::is_same_v<std::remove_cv_t<std::remove_reference_t<first_arg>>,
                       double>;

    // Vérifie que le callback retourne bien un bool
    static constexpr bool returns_bool =
        std::is_same_v<typename traits::return_type, bool>;
};

template <typename F>
inline constexpr bool is_filter_scalar_callback_v =
    callback_filter_traits<F>::is_scalar;

} // namespace details

template <typename F>
auto __filter__(F &&cb, const Serie &serie)
    -> std::enable_if_t<details::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (cb(serie.template get<double>(i), i)) {
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
auto __filter__(F &&cb, const Serie &serie)
    -> std::enable_if_t<!details::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (cb(serie.template get<Array>(i), i)) {
            indices.push_back(i);
        }
    }

    Serie result(serie.itemSize(), indices.size());
    for (uint32_t i = 0; i < indices.size(); ++i) {
        result.template set(i, serie.template get<Array>(indices[i]));
    }
    return result;
}

// --------------------------------------------------------------------

/**
 * @brief Filters elements from one or multiple Series based on a predicate,
 * i.e., **keep elements matching predicate**
 *
 * @example
 * ```cpp
 * // Scalar filtering
 * Serie s1(1, {1, 2, 3, 4, 5});
 * auto evens = filter([](double v, uint32_t) {
 *     return (int)v % 2 == 0;
 * }, s1);
 *
 * // Vector filtering
 * Serie s2({1,2,3, 4,5,6, 7,8,9});
 * auto filtered = filter(s2, [](const Array& v, uint32_t) {
 *     return v[0] > 3;  // Keep vectors where first component > 3
 * }, s2);
 * ```
 *
 * ```cpp
 * Serie stress(6, {...});
 * Serie positions(3, {...});
 *
 * auto filtered = filter(
 *     [](const Array& s, const Array& p) {
 *         return s[0] < 0 && p[2] > 0;
 *     },
 *     stress, positions
 * );
 *
 * auto filtered_stress = filtered.get(0);
 * auto filtered_positions = filtered.get(1);
 * ```
 */
template <typename F, typename... Args>
auto filter(F &&predicate, const Args &...args) {
    if constexpr (details::is_multi_series_call<Args...>::value) {
        // Multiple Series case
        static_assert(std::conjunction<details::is_serie<Args>...>::value,
                      "All arguments after predicate must be Series");

        auto counts = utils::countAndCheck(args...);

        // Collect indices that satisfy the predicate
        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < counts[0]; ++i) {
            if (predicate(args.template get<Array>(i)..., i) == true) {
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
                      "Single Serie filter requires exactly one Serie");
        const auto &serie = std::get<0>(std::forward_as_tuple(args...));
        // ... rest of original implementation ...
        return __filter__(predicate, serie);
    }
}

/**
 * @brief Creates a reusable filter function
 */
template <typename F> auto make_filter(F &&cb) {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    return [cb = std::forward<F>(cb)](const auto &serie) {
        return filter(cb, serie);
    };
}

// --------------------------------------------------------------------

/**
 * @brief Filters multiple Series simultaneously using a single predicate, i.e.,
 * **keep elements matching predicate**
 * @param predicate Function taking values from all Series and returning bool
 * @param series Variable number of Series to filter
 * @return Series containing all filtered Series
 *
 * @example
 * ```cpp
 * Serie stress(6, {...});
 * Serie positions(3, {...});
 *
 * auto filtered = filterAll(
 *     [](const Array& stress_val, const Array& pos_val) {
 *         return stress_val[0] < 0 && pos_val[2] > 0;
 *     },
 *     stress, positions
 * );
 *
 * auto filtered_stress = filtered.get(0);
 * auto filtered_positions = filtered.get(1);
 * ```
 */
template <typename Pred, typename... TheSeries>
Series filterAll(Pred &&predicate, const TheSeries &...series) {
    static_assert(details::all_are_series<TheSeries...>::value,
                  "All arguments after predicate must be Series");
    static_assert(sizeof...(series) >= 2,
                  "filterAll requires at least 2 Series");

    auto counts = utils::countAndCheck(series...);

    // Collect indices that satisfy the predicate
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < counts[0]; ++i) {
        if (predicate(series.template get<Array>(i)...)) {
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

    return Series{filter_one(series)...};
}

/**
 * @brief Creates a reusable filter function
 */
template <typename F> auto make_filterAll(F &&cb) {
    static_assert(details::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    return [cb = std::forward<F>(cb)](const auto &serie) {
        return filterAll(cb, serie);
    };
}

} // namespace df
