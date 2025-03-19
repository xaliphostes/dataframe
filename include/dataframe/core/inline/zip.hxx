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

#include <tuple>

namespace df {

/**
 * @brief Binary zip (two Series)
 */
template <typename T, typename U>
auto zip(const Serie<T> &serie1, const Serie<U> &serie2) {
    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same size for zip operation");
    }

    // Creates a Serie of tuples
    return serie1.map([&serie2](const T &value, size_t i) {
        return std::make_tuple(value, serie2[i]);
    });
}

/**
 * @brief Variadic zip (multiple Series)
 * @code
 * // Combining related data
 * Serie<double> timestamps{0.0, 1.0, 2.0};
 * Serie<double> temperatures{20.5, 21.0, 21.5};
 * auto timestamped_temps = zip(timestamps, temperatures);
 *
 * // Processing multiple series together
 * Serie<double> x{1.0, 2.0, 3.0};
 * Serie<double> y{4.0, 5.0, 6.0};
 * auto points = zip(x, y).map([](const auto& tuple) {
 *     auto [x_val, y_val] = tuple;
 *     return std::sqrt(x_val*x_val + y_val*y_val);
 * });
 * @endcode
 */
template <typename T, typename... Args>
auto zip(const Serie<T> &first, const Args &...rest) {
    // Check that all series have the same size
    const size_t size = first.size();
    if (!((rest.size() == size) && ...)) {
        throw std::runtime_error(
            "All series must have the same size for zip operation");
    }

    // Creates a Serie of tuples containing elements from all input series
    return first.map([&](const T &value, size_t i) {
        return std::make_tuple(value, rest[i]...);
    });
}

} // namespace df