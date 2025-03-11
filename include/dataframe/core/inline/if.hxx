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

// Implementation with index parameter
template <typename T, typename R>
Serie<R> if_then_else(const Serie<T> &serie,
                      std::function<bool(const T &, size_t)> condition,
                      std::function<R(const T &, size_t)> true_transform,
                      std::function<R(const T &, size_t)> false_transform) {

    return serie.map([condition, true_transform,
                      false_transform](const T &value, size_t idx) -> R {
        if (condition(value, idx)) {
            return true_transform(value, idx);
        } else {
            return false_transform(value, idx);
        }
    });
}

// Simplified implementation without index parameter
template <typename T, typename R>
Serie<R> if_then_else(const Serie<T> &serie,
                      std::function<bool(const T &)> condition,
                      std::function<R(const T &)> true_transform,
                      std::function<R(const T &)> false_transform) {

    return if_then_else<T, R>(
        serie, [condition](const T &value, size_t) { return condition(value); },
        [true_transform](const T &value, size_t) {
            return true_transform(value);
        },
        [false_transform](const T &value, size_t) {
            return false_transform(value);
        });
}

// Bind operation with index
template <typename T, typename R>
auto bind_if_then_else(std::function<bool(const T &, size_t)> condition,
                       std::function<R(const T &, size_t)> true_transform,
                       std::function<R(const T &, size_t)> false_transform) {

    return [condition, true_transform, false_transform](const Serie<T> &serie) {
        return if_then_else<T, R>(serie, condition, true_transform,
                                  false_transform);
    };
}

// Simplified bind operation without index
template <typename T, typename R>
auto bind_if_then_else(std::function<bool(const T &)> condition,
                       std::function<R(const T &)> true_transform,
                       std::function<R(const T &)> false_transform) {

    return [condition, true_transform, false_transform](const Serie<T> &serie) {
        return if_then_else<T, R>(
            serie, [condition](const T &value) { return condition(value); },
            [true_transform](const T &value) { return true_transform(value); },
            [false_transform](const T &value) {
                return false_transform(value);
            });
    };
}

} // namespace df