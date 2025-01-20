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
#include <dataframe/Serie.h>
#include <dataframe/functional/common.h>
#include <dataframe/functional/macros.h>

namespace df {
namespace detail {

// Helper pour detecter le type de retour d'une fonction
template <typename F>
struct function_reduce_traits
    : function_reduce_traits<decltype(&F::operator())> {};

template <typename C, typename R, typename... Args>
struct function_reduce_traits<R (C::*)(Args...) const> {
    using return_type = R;
    using args_tuple = std::tuple<Args...>;
};

// Type trait pour détecter les callbacks scalaires
template <typename F, typename T> struct callback_reduce_traits {
    using clean_type = std::remove_cv_t<std::remove_reference_t<F>>;
    using traits = function_reduce_traits<clean_type>;
    using first_arg = std::tuple_element_t<0, typename traits::args_tuple>;

    static constexpr bool is_scalar =
        std::is_same_v<std::remove_cv_t<std::remove_reference_t<first_arg>>, T>;
};

template <typename F, typename T>
inline constexpr bool is_reduce_scalar_callback_v =
    callback_reduce_traits<F, T>::is_scalar;

} // namespace detail

/**
 * @brief Reduces a GenSerie to either a T or a GenSerie<T>
 */
template <typename F, typename T>
auto reduce(F &&cb, const GenSerie<T> &serie, T init)
    -> std::enable_if_t<detail::is_reduce_scalar_callback_v<F, T>, T> {
    if (serie.itemSize() != 1) {
        throw std::invalid_argument(
            "Scalar reduce can only be used with Serie of itemSize 1");
    }

    T result = init;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        result = cb(result, serie.value(i), i);
    }
    return result;
}

template <typename F, typename T>
auto reduce(F &&cb, const GenSerie<T> &serie, const std::vector<T> &init)
    -> std::enable_if_t<!detail::is_reduce_scalar_callback_v<F, T>,
                        GenSerie<T>> {
    if (init.size() != serie.itemSize()) {
        throw std::invalid_argument(
            "Initial vector size must match serie itemSize");
    }

    std::vector<T> result = init;
    for (uint32_t i = 0; i < serie.count(); ++i) {
        result = cb(result, serie.array(i), i);
    }
    return GenSerie<T>(result.size(), result);
}

/**
 * @brief Creates a reusable reduce function
 */
template <typename F, typename T> auto make_reduce(F &&cb, T init) {
    static_assert(detail::is_reduce_scalar_callback_v<F, T>,
                  "Scalar reduce requires a callback taking scalar values");
    return [cb = std::forward<F>(cb), init](const auto &serie) {
        return reduce(cb, serie, init);
    };
}

template <typename F, typename T>
auto make_reduce(F &&cb, const std::vector<T> &init) {
    static_assert(!detail::is_reduce_scalar_callback_v<F, T>,
                  "Vector reduce requires a callback taking vectors");
    return [cb = std::forward<F>(cb), init](const auto &serie) {
        return reduce(cb, serie, init);
    };
}


/*
// Somme des valeurs
template<typename T>
T sum(const GenSerie<T>& serie) {
    return reduce([](T acc, T val) {
        return acc + val;
    }, serie, T{});
}

// Produit des valeurs
template<typename T>
T product(const GenSerie<T>& serie) {
    return reduce([](T acc, T val) {
        return acc * val;
    }, serie, T{1});
}

// Min (version scalaire)
template<typename T>
T min(const GenSerie<T>& serie) {
    if (serie.count() == 0) throw std::invalid_argument("Empty serie");
    return reduce([](T acc, T val) {
        return std::min(acc, val);
    }, serie, serie.value(0));
}

// Max (version scalaire)
template<typename T>
T max(const GenSerie<T>& serie) {
    if (serie.count() == 0) throw std::invalid_argument("Empty serie");
    return reduce([](T acc, T val) {
        return std::max(acc, val);
    }, serie, serie.value(0));
}
*/

} // namespace df

