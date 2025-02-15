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

#include <limits>
#include <tuple>
#include <type_traits>
#include <dataframe/meta.h>

namespace df {

namespace details {

// Helper for arithmetic types
template <typename T>
auto get_element_bounds_impl(const T &value, std::true_type) {
    return std::make_tuple(value, value);
}

// Helper for array types (like Vector3D)
template <typename T, std::size_t N>
auto get_array_bounds(const std::array<T, N> &arr) {
    T min_val = std::numeric_limits<T>::max();
    T max_val = std::numeric_limits<T>::lowest();

    for (const auto &val : arr) {
        min_val = std::min(min_val, val);
        max_val = std::max(max_val, val);
    }
    return std::make_tuple(min_val, max_val);
}

// Helper for container types
template <typename T>
auto get_element_bounds_impl(const T &value, std::false_type) {
    if constexpr (details::is_std_array<T>::value) {
        return get_array_bounds(value);
    } else if constexpr (is_container<T>::value) {
        using ElementType = typename container_value_type<T>::type;
        ElementType min_val = std::numeric_limits<ElementType>::max();
        ElementType max_val = std::numeric_limits<ElementType>::lowest();

        for (const auto &val : value) {
            min_val = std::min(min_val, val);
            max_val = std::max(max_val, val);
        }
        return std::make_tuple(min_val, max_val);
    } else {
        static_assert(std::is_arithmetic<T>::value || is_container<T>::value,
                      "Type must be either arithmetic or a container type");
        return std::make_tuple(T{}, T{}); // Unreachable
    }
}

// Main dispatch function for getting bounds of a single element
template <typename T> auto get_element_bounds(const T &value) {
    return get_element_bounds_impl(value, std::is_arithmetic<T>{});
}

} // namespace details

// Main bounds function returning both min and max
template <typename T> inline auto bounds(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot compute bounds of empty serie");
    }

    // Get bounds type from first element
    auto [init_min, init_max] = details::get_element_bounds(serie[0]);

    using BoundType = decltype(init_min);
    BoundType min_val = init_min;
    BoundType max_val = init_max;

    // Process remaining elements
    for (size_t i = 1; i < serie.size(); ++i) {
        auto [elem_min, elem_max] = details::get_element_bounds(serie[i]);
        min_val = std::min(min_val, elem_min);
        max_val = std::max(max_val, elem_max);
    }

    return std::make_tuple(min_val, max_val);
}

// Convenience function for minimum only
template <typename T> inline auto min(const Serie<T> &serie) {
    auto [min_val, _] = bounds(serie);
    return min_val;
}

// Convenience function for maximum only
template <typename T> inline auto max(const Serie<T> &serie) {
    auto [_, max_val] = bounds(serie);
    return max_val;
}

// Pipeline operation support
// template <typename T> struct bounds_binder {
//     auto operator()(const Serie<T> &serie) const { return bounds(serie); }
// };

// template <typename T> struct min_binder {
//     auto operator()(const Serie<T> &serie) const { return min(serie); }
// };

// template <typename T> struct max_binder {
//     auto operator()(const Serie<T> &serie) const { return max(serie); }
// };

// Helper functions to create binders
inline auto bind_bounds() {
    return []<typename T>(const Serie<T> &serie) { return bounds(serie); };
}

inline auto bind_min() {
    return []<typename T>(const Serie<T> &serie) { return min(serie); };
}

inline auto bind_max() {
    return []<typename T>(const Serie<T> &serie) { return max(serie); };
}

} // namespace df