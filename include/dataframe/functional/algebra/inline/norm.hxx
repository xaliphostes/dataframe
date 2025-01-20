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
namespace algebra {

template <typename T> using IsFloating = df::details::IsFloating<T>;

/**
 * Calculate squared norm for a single number or vector
 * For scalar: returns x*x
 * For vector: returns sum of squares of components
 */
template <typename T> inline IsFloating<T> norm2(const std::vector<T> &v) {
    T sum = 0;
    for (const auto &x : v) {
        sum += x * x;
    }
    return sum;
}

template <typename T> inline IsFloating<T> norm2(T x) { return x * x; }

/**
 * Calculate squared norm for each element in a GenSerie
 * Returns a GenSerie with itemSize=1 containing the squared norms
 */
template <typename T>
inline IsSerieFloating<T> norm2(const GenSerie<T> &serie) {
    // Résultat sera toujours une série scalaire
    GenSerie<T> result(1, serie.count());

    if (serie.itemSize() == 1) {
        // Pour série scalaire, carré de chaque valeur
        for (uint32_t i = 0; i < serie.count(); ++i) {
            result.setValue(i, norm2(serie.value(i)));
        }
    } else {
        // Pour série vectorielle, somme des carrés des composantes
        for (uint32_t i = 0; i < serie.count(); ++i) {
            result.setValue(i, norm2(serie.array(i)));
        }
    }

    return result;
}

// -----------------------------------------------

/**
 * Calculate norm for a single number or vector
 * For scalar: returns abs(x)
 * For vector: returns euclidean norm (sqrt of sum of squares)
 */
template <typename T> inline IsFloating<T> norm(const std::vector<T> &v) {
    return std::sqrt(norm2(v));
}

template <typename T> inline IsFloating<T> norm(T x) { return std::abs(x); }

/**
 * Calculate norm for each element in a GenSerie
 * Returns a GenSerie with itemSize=1 containing the norms
 */
template <typename T> inline IsSerieFloating<T> norm(const GenSerie<T> &serie) {
    // Résultat sera toujours une série scalaire
    GenSerie<T> result(1, serie.count());

    if (serie.itemSize() == 1) {
        // Pour série scalaire, valeur absolue
        for (uint32_t i = 0; i < serie.count(); ++i) {
            result.setValue(i, norm(serie.value(i)));
        }
    } else {
        // Pour série vectorielle, norme euclidienne
        for (uint32_t i = 0; i < serie.count(); ++i) {
            result.setValue(i, norm(serie.array(i)));
        }
    }

    return result;
}

} // namespace algebra
} // namespace df