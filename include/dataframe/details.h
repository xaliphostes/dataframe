#pragma once
#include "Serie.h"

namespace df {

  namespace details {
    template <typename T> struct is_serie : std::false_type {};

    template <> struct is_serie<Serie> : std::true_type {};

    template <typename T>
    inline constexpr bool is_serie_v =
        is_serie<std::remove_reference_t<T>>::value;

    // Helper pour obtenir le count d'une Serie
    template <typename T> size_t get_count(const T &t) {
      if constexpr (is_serie_v<T>) {
        return t.count();
      } else {
        throw std::invalid_argument("Parameter must be a Serie");
      }
    }

    template <typename... Ts> struct all_are_series;

    template <> struct all_are_series<> : std::true_type {};

    template <typename T, typename... Rest>
    struct all_are_series<T, Rest...>
        : std::bool_constant<is_serie_v<T> && all_are_series<Rest...>::value> {
    };
  } // namespace detail

} // namespace df