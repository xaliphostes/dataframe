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
#include <dataframe/Serie.h>
#include <dataframe/details.h>

namespace df {

  namespace detail {

    // Helper pour détecter le type de retour d'une fonction
    template <typename F>
    struct function_filter_traits
        : function_filter_traits<decltype(&F::operator())> {};

    template <typename C, typename R, typename... Args>
    struct function_filter_traits<R (C::*)(Args...) const> {
      using return_type = R;
      using args_tuple = std::tuple<Args...>;
    };

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
  } // namespace detail

  /**
   * @brief Filters elements from a Serie based on a predicate
   *
   * @example
   * ```cpp
   * // Scalar filtering
   * Serie s1(1, {1, 2, 3, 4, 5});
   * auto evens = filter(s1, [](double v, uint32_t) {
   *     return (int)v % 2 == 0;
   * });
   *
   * // Vector filtering
   * Serie s2(3, {1,2,3, 4,5,6, 7,8,9});
   * auto filtered = filter(s2, [](const Array& v, uint32_t) {
   *     return v[0] > 3;  // Keep vectors where first component > 3
   * });
   * ```
   */
  template <typename F>
  auto filter(const Serie &serie, F &&cb)
      -> std::enable_if_t<detail::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(detail::callback_filter_traits<F>::returns_bool,
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
  auto filter(const Serie &serie, F &&cb)
      -> std::enable_if_t<!detail::is_filter_scalar_callback_v<F>, Serie> {
    static_assert(detail::callback_filter_traits<F>::returns_bool,
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

  /**
   * @brief Creates a reusable filter function
   */
  template <typename F> auto make_filter(F &&cb) {
    static_assert(detail::callback_filter_traits<F>::returns_bool,
                  "Filter predicate must return bool");

    return [cb = std::forward<F>(cb)](const auto &serie) {
      return filter(serie, cb);
    };
  }

  // --------------------------------------------------------------------

//   namespace details {
//     // Helper pour détecter si un type est une Serie
//     template <typename T> struct is_serie : std::false_type {};

//     template <> struct is_serie<Serie> : std::true_type {};

//     template <typename T>
//     inline constexpr bool is_serie_v =
//         is_serie<std::remove_reference_t<T>>::value;

//     // Helper pour obtenir le count d'une Serie
//     template <typename T> size_t get_count(const T &t) {
//       if constexpr (is_serie_v<T>) {
//         return t.count();
//       } else {
//         throw std::invalid_argument("Parameter must be a Serie");
//       }
//     }

//     template <typename... Ts> struct all_are_series;

//     template <> struct all_are_series<> : std::true_type {};

//     template <typename T, typename... Rest>
//     struct all_are_series<T, Rest...>
//         : std::bool_constant<is_serie_v<T> && all_are_series<Rest...>::value> {
//     };
//   } // namespace details

  /**
   * @brief Filters multiple Series simultaneously using a single predicate
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

} // namespace df
