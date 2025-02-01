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

namespace df {

namespace details {

// Helper for negating arithmetic types
template <typename T> auto negate_element_impl(const T &value, std::true_type) {
    return -value;
}

// Helper for array types (like Vector3D)
template <typename T, std::size_t N>
auto negate_array(const std::array<T, N> &arr) {
    std::array<T, N> result;
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = -arr[i];
    }
    return result;
}

// Helper for container types
template <typename T>
auto negate_element_impl(const T &value, std::false_type) {
    if constexpr (is_std_array<T>::value) {
        return negate_array(value);
    } else if constexpr (is_container<T>::value) {
        T result;
        if constexpr (is_resizable_container<T>::value) {
            result.reserve(value.size());
        }
        for (size_t i = 0; i < value.size(); ++i) {
            if constexpr (is_resizable_container<T>::value) {
                result.push_back(-value[i]);
            } else {
                result[i] = -value[i];
            }
        }
        return result;
    } else {
        static_assert(std::is_arithmetic<T>::value || is_container<T>::value,
                      "Type must be either arithmetic or a container type");
        return T{}; // Unreachable, just to satisfy compiler
    }
}

// Main dispatch function for negation
template <typename T> auto negate_element(const T &value) {
    return negate_element_impl(value, std::is_arithmetic<T>{});
}

} // namespace details

// Main negate function
template <typename T> inline auto negate(const Serie<T> &serie) {
    return serie.map(
        [](const T &value, size_t) { return details::negate_element(value); });
}

// Pipeline operation support
// template <typename T> struct negate_binder {
//     Serie<T> operator()(const Serie<T> &serie) const { return negate(serie); }
// };

// Helper function to create binder
inline auto bind_negate() {
    return []<typename T>(const Serie<T> &serie) { return negate(serie); };
}

} // namespace df

// --------------------------------------------------

// Operator overload for unary minus
template <typename T> inline df::Serie<T> operator-(const df::Serie<T> &serie)  {
    return df::negate(serie);
}
