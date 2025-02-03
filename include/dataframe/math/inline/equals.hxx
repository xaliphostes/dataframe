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

// Default tolerance for floating point comparisons
template <typename T> constexpr inline T default_tolerance() {
    if constexpr (std::is_same_v<T, float>) {
        return 1e-6f;
    } else if constexpr (std::is_same_v<T, double>) {
        return 1e-12;
    } else if constexpr (std::is_same_v<T, long double>) {
        return 1e-15l;
    } else {
        return T{0};
    }
}

// Helper function to check if two values are equal within tolerance
template <typename T>
inline bool are_equal(const T &a, const T &b,
                      const T &tol = default_tolerance<T>()) {
    if constexpr (std::is_floating_point_v<T>) {
        return std::abs(a - b) <= tol;
    } else {
        return a == b;
    }
}

// Main equals function for Series comparison
template <typename T>
inline bool equals(const Serie<T> &a, const Serie<T> &b, const T &tol) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (!are_equal(a[i], b[i], tol)) {
            return false;
        }
    }

    return true;
}

// Pipeline operation binder
template <typename T> struct equals_binder {
    const Serie<T> &other;
    T tolerance;

    equals_binder(const Serie<T> &s, const T &tol) : other(s), tolerance(tol) {}

    bool operator()(const Serie<T> &serie) const {
        return equals(serie, other, tolerance);
    }
};

// Helper function to create binder
template <typename T>
inline auto bind_equals(const Serie<T> &other, const T &tol) {
    return equals_binder<T>(other, tol);
}

} // namespace df
