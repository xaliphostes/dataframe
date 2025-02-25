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

template <typename T>
inline Serie<T> slice(const Serie<T> &serie, size_t start, size_t end) {
    if (start > end) {
        throw std::invalid_argument(
            "Start index cannot be greater than end index");
    }
    if (end > serie.size()) {
        throw std::out_of_range("End index out of bounds");
    }

    std::vector<T> sliced_data(serie.data().begin() + start,
                               serie.data().begin() + end);
    return Serie<T>(sliced_data);
}

template <typename T> inline Serie<T> slice(const Serie<T> &serie, size_t end) {
    return slice(serie, 0, end);
}

template <typename T>
inline Serie<T> slice(const Serie<T> &serie, size_t start, size_t end,
                      size_t step) {
    if (start > end) {
        throw std::invalid_argument(
            "Start index cannot be greater than end index");
    }
    if (end > serie.size()) {
        throw std::out_of_range("End index out of bounds");
    }
    if (step == 0) {
        throw std::invalid_argument("Step cannot be zero");
    }

    std::vector<T> sliced_data;
    sliced_data.reserve((end - start + step - 1) / step);

    for (size_t i = start; i < end; i += step) {
        sliced_data.push_back(serie[i]);
    }

    return Serie<T>(sliced_data);
}

template <typename... Args> inline auto bind_slice(Args... args) {
    return [args...](const auto &serie) { return slice(serie, args...); };
}

} // namespace df