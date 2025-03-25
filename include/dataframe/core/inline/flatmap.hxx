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

#include <dataframe/utils/utils.h>
#include <functional>
#include <type_traits>
#include <vector>

namespace df {

template <typename T, typename R>
inline Serie<R> flatMap(const Serie<T> &serie,
                        std::function<Serie<R>(const T &, size_t)> callback) {
    std::vector<R> result;

    for (size_t i = 0; i < serie.size(); ++i) {
        auto mapped = callback(serie[i], i);
        for (size_t j = 0; j < mapped.size(); ++j) {
            result.push_back(mapped[j]);
        }
    }

    return Serie<R>(result);
}

template <typename T, typename R>
inline Serie<R> flatMap(const Serie<T> &serie,
                        std::function<Serie<R>(const T &)> callback) {
    return flatMap<T, R>(
        serie, [callback](const T &value, size_t) { return callback(value); });
}

template <typename T, typename R>
inline auto bind_flatMap(std::function<Serie<R>(const T &, size_t)> callback) {
    return [callback](const Serie<T> &serie) {
        return flatMap<T, R>(serie, callback);
    };
}

template <typename T, typename R>
inline auto bind_flatMap(std::function<Serie<R>(const T &)> callback) {
    return [callback](const Serie<T> &serie) {
        return flatMap<T, R>(serie, [callback](const T &value, size_t) {
            return callback(value);
        });
    };
}

} // namespace df
