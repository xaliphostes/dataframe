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
inline Serie<T> concat(const std::vector<Serie<T>> &series) {
    // Calculate total size needed
    size_t total_size = 0;
    for (const auto &serie : series) {
        total_size += serie.size();
    }

    // Create result array with reserved capacity
    std::vector<T> result;
    result.reserve(total_size);

    // Copy all elements from each serie
    for (const auto &serie : series) {
        const auto &data = serie.data();
        result.insert(result.end(), data.begin(), data.end());
    }

    return Serie<T>(result);
}

// Variadic template version for ease of use
template <typename T, typename... Args>
inline Serie<T> concat(const Serie<T> &first, const Args &...args) {
    std::vector<Serie<T>> series{first, args...};
    return concat(series);
}

template <typename... Args> inline auto bind_concat(const Args &...series) {
    return [series...](const auto &input_serie) {
        return concat(input_serie, series...);
    };
}

} // namespace df