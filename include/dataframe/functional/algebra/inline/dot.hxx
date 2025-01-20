
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

#include <dataframe/functional/map.h>

namespace df {
namespace algebra {

/**
 * @brief Compute dot product between two series of vectors
 */
template <typename T>
inline GenSerie<T> dot(const GenSerie<T> &a, const GenSerie<T> &b) {
    if (a.count() != b.count() || a.itemSize() != b.itemSize()) {
        throw std::invalid_argument("Series must have same dimensions");
    }

    return df::map(
        [&b](const std::vector<T> &a_vec, uint32_t i) -> T {
            T result = 0;
            const auto &b_vec = b.array(i);
            for (uint32_t k = 0; k < a_vec.size(); ++k) {
                result += a_vec[k] * b_vec[k];
            }
            return result;
        },
        a);
}

/**
 * @brief Compute dot product between a series of vectors and a constant vector
 */
// template<typename T>
// inline GenSerie<T> dot(const GenSerie<T>& a, const std::vector<T>& b) {
//     if (a.itemSize() != b.size()) {
//         throw std::invalid_argument("Vector dimensions must match series
//         itemSize");
//     }

//     return df::map(
//         [&b](const std::vector<T>& a_vec, uint32_t) -> T {
//             T result = 0;
//             for (uint32_t k = 0; k < a_vec.size(); ++k) {
//                 result += a_vec[k] * b[k];
//             }
//             return result;
//         },
//         a
//     );
// }
template <typename T>
GenSerie<T> dot(const GenSerie<T> &a, const std::vector<T> &b) {
    if (!a.isValid() || a.itemSize() != 3) {
        return GenSerie<T>();
    }

    if (b.size() != 3) {
        return GenSerie<T>();
    }

    // Create a functor that maps the vector series to scalar series
    auto dot_product = [&b](const auto &c, uint32_t) -> T {
        return c[0] * b[0] + c[1] * b[1] +
               c[2] * b[2];
    };

    GenSerie<T> result(1, a.count());
    for (uint32_t i = 0; i < a.count(); ++i) {
        result.setValue(i, dot_product(a.array(i), i));
    }
    return result;
}

/**
 * @brief Create dot operation that can be used with the pipe operator
 */
template <typename T> inline auto make_dot(const std::vector<T> &vec) {
    return [&vec](const GenSerie<T> &serie) { return dot(serie, vec); };
}

template <typename T> inline auto make_dot(const GenSerie<T> &serie) {
    return [&serie](const GenSerie<T> &other) { return dot(other, serie); };
}

} // namespace algebra
} // namespace df