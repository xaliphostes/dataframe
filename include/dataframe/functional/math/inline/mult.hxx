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
namespace math {

// Multiplication par un scalaire
template <typename T>
inline details::IsSerieFloating<T> mult(const GenSerie<T> &serie, T d) {
    return df::map(
        [d](const auto &a, uint32_t) -> std::decay_t<decltype(a)> {
            if constexpr (std::is_same_v<std::decay_t<decltype(a)>, T>) {
                return a * d;
            } else {
                auto r = a;
                for (uint32_t j = 0; j < a.size(); ++j) {
                    r[j] *= d;
                }
                return r;
            }
        },
        serie);
}

template <typename T>
inline details::IsSerieFloating<T> mult(const GenSerie<T> &serie,
                                const GenSerie<T> &multiplier) {
    // Verify counts match
    if (serie.count() != multiplier.count()) {
        throw std::invalid_argument("count of serie (" +
                                    std::to_string(serie.count()) +
                                    ") differs from count of multiplier (" +
                                    std::to_string(multiplier.count()) + ")");
    }

    // Both series must be scalar or vector
    if (serie.itemSize() == 1 && multiplier.itemSize() != 1) {
        throw std::invalid_argument("itemSize of multiplier should be 1 since "
                                    "serie has itemSize of 1. Got " +
                                    std::to_string(multiplier.itemSize()));
    }

    return df::map(
        [&multiplier](const auto &a, uint32_t i) -> std::decay_t<decltype(a)> {
            if constexpr (std::is_same_v<std::decay_t<decltype(a)>, T>) {
                // Scalar case
                return a * multiplier.value(i);
            } else {
                // Vector case
                if (multiplier.itemSize() == 1) {
                    // Multiply vector by scalar
                    T d = multiplier.value(i);
                    auto r = a;
                    for (uint32_t j = 0; j < a.size(); ++j) {
                        r[j] *= d;
                    }
                    return r;
                } else {
                    // Multiply vector by vector
                    if (multiplier.itemSize() != a.size()) {
                        throw std::invalid_argument(
                            "itemSize of multiplier should be the same as the "
                            "serie's itemSize. Got " +
                            std::to_string(multiplier.itemSize()));
                    }
                    auto d = multiplier.array(i);
                    auto r = a;
                    for (uint32_t j = 0; j < r.size(); ++j) {
                        r[j] *= d[j];
                    }
                    return r;
                }
            }
        },
        serie);
}

// Helper pour créer un opérateur de multiplication
template <typename T> inline auto make_mult(T scalar) {
    return [scalar](const auto &serie) { return mult(serie, scalar); };
}

template <typename T> inline auto make_mult(const GenSerie<T> &multiplier) {
    return [&multiplier](const auto &serie) { return mult(serie, multiplier); };
}

} // namespace math
} // namespace df