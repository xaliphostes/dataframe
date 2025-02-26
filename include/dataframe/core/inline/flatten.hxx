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

#include <deque>
#include <list>
#include <type_traits>
#include <vector>

namespace df {

namespace detail {

// Type trait to detect if a type is an iterable container (has begin/end
// methods)
template <typename T, typename = void> struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))>>
    : std::true_type {};

template <typename T> constexpr bool is_iterable_v = is_iterable<T>::value;

// Type trait to check if T is a Serie
template <typename T> struct is_serie : std::false_type {};

template <typename U> struct is_serie<Serie<U>> : std::true_type {};

template <typename T> constexpr bool is_serie_v = is_serie<T>::value;

// Determine the element type of a container
template <typename Container> struct element_type {
    using type =
        typename std::decay_t<decltype(*std::begin(std::declval<Container>()))>;
};

// Special case for Serie
template <typename T> struct element_type<Serie<T>> {
    using type = T;
};

// Special case for nested Serie
template <typename T> struct element_type<Serie<Serie<T>>> {
    using type = T;
};

template <typename Container>
using element_type_t = typename element_type<Container>::type;

// Helper function to determine the inner-most element type for recursively
// nested containers
template <typename Container> struct innermost_element_type {
    using inner_type = element_type_t<Container>;

    // If the element type is itself a container (and not string), recurse
    using type = typename std::conditional_t<
        is_iterable_v<inner_type> && !std::is_same_v<inner_type, std::string>,
        innermost_element_type<inner_type>,
        std::type_identity<inner_type>>::type;
};

template <typename Container>
using innermost_element_type_t =
    typename innermost_element_type<Container>::type;

// Function to add elements to result vector, handling nested containers
// recursively
template <typename T, typename ResultElemType>
void flatten_impl(const T &item, std::vector<ResultElemType> &result) {
    if constexpr (std::is_convertible_v<T, ResultElemType>) {
        // For primitive types or directly convertible types, add to result
        result.push_back(static_cast<ResultElemType>(item));
    } else if constexpr (is_iterable_v<T> && !std::is_same_v<T, std::string>) {
        // For container types, recursively flatten each element
        for (const auto &elem : item) {
            flatten_impl(elem, result);
        }
    } else {
        static_assert(
            std::is_convertible_v<T, ResultElemType>,
            "Cannot flatten this type hierarchy - incompatible element types");
    }
}

// Special case for Serie of Serie
template <typename T, typename ResultElemType>
void flatten_impl(const Serie<Serie<T>> &series_of_series,
                  std::vector<ResultElemType> &result) {
    for (size_t i = 0; i < series_of_series.size(); ++i) {
        const auto &inner_serie = series_of_series[i];
        for (size_t j = 0; j < inner_serie.size(); ++j) {
            result.push_back(static_cast<ResultElemType>(inner_serie[j]));
        }
    }
}

} // namespace detail

// ----------------------------------------------------------

/**
 * Specialized implementation for Serie of Serie (common case)
 */
template <typename T>
Serie<T> flatten(const Serie<Serie<T>> &series_of_series) {
    // First, calculate the total size needed
    size_t total_size = 0;
    for (size_t i = 0; i < series_of_series.size(); ++i) {
        total_size += series_of_series[i].size();
    }

    // Create a result vector with the right capacity
    std::vector<T> result;
    result.reserve(total_size);

    // Fill the result by iterating through each inner serie
    for (size_t i = 0; i < series_of_series.size(); ++i) {
        const auto &inner_serie = series_of_series[i];
        for (size_t j = 0; j < inner_serie.size(); ++j) {
            result.push_back(inner_serie[j]);
        }
    }

    return Serie<T>(result);
}

/**
 * Implementation for Serie of containers (vector, list, etc.)
 */
template <typename T>
auto flatten(const Serie<T> &serie) -> typename std::enable_if_t<
    detail::is_iterable_v<T> && !std::is_same_v<T, std::string> &&
        !detail::is_serie_v<T>,
    Serie<typename detail::innermost_element_type_t<T>>> {
    using ResultType = typename detail::innermost_element_type_t<T>;
    std::vector<ResultType> result;

    // Estimate capacity
    size_t estimated_size = serie.size() * 4; // Simple heuristic
    result.reserve(estimated_size);

    // Process each container in the serie
    for (size_t i = 0; i < serie.size(); ++i) {
        detail::flatten_impl(serie[i], result);
    }

    return Serie<ResultType>(result);
}

/**
 * Fallback implementation for non-container types or strings
 * Just returns the original serie
 */
template <typename T>
auto flatten(const Serie<T> &serie) -> typename std::enable_if_t<
    !detail::is_iterable_v<T> || std::is_same_v<T, std::string>, Serie<T>> {
    return serie;
}

/**
 * Function to flatten an arbitrary container (non-Serie)
 */
template <typename Container,
          typename = std::enable_if_t<!detail::is_serie_v<Container> &&
                                      detail::is_iterable_v<Container> &&
                                      !std::is_same_v<Container, std::string>>>
auto flatten(const Container &container) {
    using ElementType = detail::element_type_t<Container>;
    using ResultType = detail::innermost_element_type_t<Container>;

    std::vector<ResultType> result;

    // Estimate size if possible
    if constexpr (requires { container.size(); }) {
        result.reserve(container.size() * 2);
    }

    // Recursively flatten the container
    detail::flatten_impl(container, result);

    return Serie<ResultType>(result);
}

// -----------------------------------------------------------------
// flattenDeep implementation
// -----------------------------------------------------------------

namespace detail {
// Type level integers for template metaprogramming
template <int N> struct Int {
    static constexpr int value = N;
};

// Helper to recursively flatten a serie up to a specific depth
template <typename T, int Depth> struct FlattenDepth {
    static auto apply(const Serie<T> &serie) {
        // Base case: depth is 0 or 1
        if constexpr (Depth <= 1) {
            if constexpr (Depth == 0) {
                return serie;
            } else {
                return flatten(serie);
            }
        } else {
            // Recursive case: flatten one level, then continue
            using FlattenedType = decltype(flatten(serie));
            using ElementType = element_type_t<FlattenedType>;

            // Only recurse if the element type is a container and not a string
            if constexpr (is_iterable_v<ElementType> &&
                          !std::is_same_v<ElementType, std::string>) {
                auto one_level = flatten(serie);
                return FlattenDepth<ElementType, Depth - 1>::apply(one_level);
            } else {
                // No more nesting possible, just return one level flatten
                return flatten(serie);
            }
        }
    }
};
} // namespace detail

/**
 * Specialized flattenDeep for Serie of vector of vector
 */
template <typename T>
auto flattenDeep(const Serie<std::vector<std::vector<T>>> &nested_serie,
                 int depth = INT_MAX) {
    if (depth <= 0) {
        return nested_serie;
    } else if (depth == 1) {
        // Flatten to Serie<std::vector<T>>
        std::vector<std::vector<T>> result;
        for (size_t i = 0; i < nested_serie.size(); ++i) {
            for (const auto &inner_vec : nested_serie[i]) {
                result.push_back(inner_vec);
            }
        }
        return Serie<std::vector<T>>(result);
    } else {
        // Flatten completely to Serie<T>
        std::vector<T> result;
        for (size_t i = 0; i < nested_serie.size(); ++i) {
            for (const auto &inner_vec : nested_serie[i]) {
                for (const auto &value : inner_vec) {
                    result.push_back(value);
                }
            }
        }
        return Serie<T>(result);
    }
}

/**
 * Generic flattenDeep function - uses hard-coded implementation for common
 * depths to avoid complicated template metaprogramming
 */
template <typename T>
auto flattenDeep(const Serie<T> &nested_serie, int depth) {
    if (depth <= 0) {
        return nested_serie;
    }

    // Just use flatten for depth 1
    if (depth == 1) {
        return flatten(nested_serie);
    }

    // For deeper levels, use a metaprogramming approach with compile-time
    // depths
    if constexpr (detail::is_iterable_v<T> && !std::is_same_v<T, std::string>) {
        // We handle up to a reasonable max depth
        int actual_depth = std::min(depth, 10); // Cap at 10 for sanity

        switch (actual_depth) {
        case 2:
            return detail::FlattenDepth<T, 2>::apply(nested_serie);
        case 3:
            return detail::FlattenDepth<T, 3>::apply(nested_serie);
        case 4:
            return detail::FlattenDepth<T, 4>::apply(nested_serie);
        case 5:
            return detail::FlattenDepth<T, 5>::apply(nested_serie);
        default:
            return detail::FlattenDepth<T, 10>::apply(nested_serie);
        }
    } else {
        // For non-container types, just use flatten
        return flatten(nested_serie);
    }
}

// Helper function to create a bound flatten operation for use in pipe
inline auto bind_flatten() {
    return [](const auto &serie) { return flatten(serie); };
}

// Helper function to create a bound flattenDeep operation for use in pipe
inline auto bind_flattenDeep(int depth) {
    return [depth](const auto &serie) { return flattenDeep(serie, depth); };
}

} // namespace df