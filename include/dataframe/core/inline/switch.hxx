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

template <typename T, typename R>
Serie<R>
switch_case(const Serie<T> &serie,
            const std::vector<std::pair<std::function<bool(const T &)>,
                                        std::function<R(const T &)>>> &cases,
            std::function<R(const T &)> default_case) {

    return serie.map([&cases, &default_case](const T &value, size_t idx) -> R {
        for (const auto &[condition, transform] : cases) {
            if (condition(value)) {
                return transform(value);
            }
        }
        return default_case(value);
    });
}

template <typename T, typename R>
auto bind_switch_case(
    const std::vector<std::pair<std::function<bool(const T &)>,
                                std::function<R(const T &)>>> &cases,
    std::function<R(const T &)> default_case) {

    return [cases, default_case](const Serie<T> &serie) {
        return switch_case<T, R>(serie, cases, default_case);
    };
}

} // namespace df