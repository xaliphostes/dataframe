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

#pragma once
#include <array>
#include <dataframe/Serie.h>
#include <dataframe/functional/common.h>
#include <tuple>

namespace df {
namespace details {

// // Type trait pour détecter les prédicats scalaires
// template <typename F, typename T, typename Enable = void>
// struct accepts_scalar : std::false_type {};

// template <typename F, typename T>
// struct accepts_scalar<F, T,
//                       std::void_t<decltype(std::declval<F>()(
//                           std::declval<T>(), std::declval<uint32_t>()))>>
//     : std::true_type {};

// // Type trait pour détecter les prédicats vectoriels
// template <typename F, typename T, typename Enable = void>
// struct accepts_vector : std::false_type {};

// template <typename F, typename T>
// struct accepts_vector<
//     F, T,
//     std::void_t<decltype(std::declval<F>()(
//         std::declval<const std::vector<T> &>(), std::declval<uint32_t>()))>>
//     : std::true_type {};

} // namespace details

// ----------------------------------------------------------------

// Structure pour stocker les résultats de recherche
template <typename T> struct FindResult {
    std::vector<uint32_t> indices;      // indices des éléments trouvés
    std::vector<T> values;              // valeurs trouvées (pour scalaires)
    std::vector<std::vector<T>> arrays; // valeurs trouvées (pour vecteurs)
    bool isScalar;                      // true si recherche scalaire

    // Helper pour obtenir le nombre de résultats
    size_t size() const { return indices.size(); }
    bool empty() const { return indices.empty(); }

    // Accesseurs pour les valeurs
    const T &getValue(size_t i = 0) const {
        if (!isScalar)
            throw std::runtime_error("Not a scalar result");
        return values.at(i);
    }

    const std::vector<T> &getArray(size_t i = 0) const {
        if (isScalar)
            throw std::runtime_error("Not a vector result");
        return arrays.at(i);
    }
};

// Version scalaire de find
template <typename F, typename T>
auto find(F &&predicate, const GenSerie<T> &serie)
    -> std::enable_if_t<details::accepts_scalar<F, T>::value, FindResult<T>> {
    if (serie.itemSize() != 1) {
        throw std::invalid_argument(
            "Scalar predicate can only be used with Serie of itemSize 1");
    }

    FindResult<T> result;
    result.isScalar = true;

    for (uint32_t i = 0; i < serie.count(); ++i) {
        if (predicate(serie.value(i), i)) {
            result.indices.push_back(i);
            result.values.push_back(serie.value(i));
        }
    }

    return result;
}

// Version vectorielle de find
template <typename F, typename T>
auto find(F &&predicate, const GenSerie<T> &serie)
    -> std::enable_if_t<details::accepts_vector<F, T>::value, FindResult<T>> {
    FindResult<T> result;
    result.isScalar = false;

    for (uint32_t i = 0; i < serie.count(); ++i) {
        auto values = serie.itemSize() > 1 ? serie.array(i)
                                           : std::vector<T>{serie.value(i)};

        if (predicate(values, i)) {
            result.indices.push_back(i);
            result.arrays.push_back(values);
        }
    }

    return result;
}

// ----------------------------------------------------------------

/**
 * @brief Finds all elements matching a predicate
 * @return GenSerie containing only matching elements
 */
template<typename F, typename T>
GenSerie<T> findAll(F&& predicate, const GenSerie<T>& serie) {
    std::vector<uint32_t> indices;

    if constexpr (details::is_scalar_callback_v<F, T>) {
        // Version scalaire
        if (serie.itemSize() != 1) {
            throw std::invalid_argument("Cannot use scalar callback for Serie with itemSize > 1");
        }

        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (predicate(serie.value(i), i)) {
                indices.push_back(i);
            }
        }
    } else {
        // Version vectorielle
        for (uint32_t i = 0; i < serie.count(); ++i) {
            auto values = serie.itemSize() > 1 ? 
                         serie.array(i) : 
                         std::vector<T>{serie.value(i)};

            if (predicate(values, i)) {
                indices.push_back(i);
            }
        }
    }

    // Création de la série résultat
    GenSerie<T> result(serie.itemSize(), indices.size());
    for (uint32_t i = 0; i < indices.size(); ++i) {
        if (serie.itemSize() == 1) {
            result.setValue(i, serie.value(indices[i]));
        } else {
            result.setArray(i, serie.array(indices[i]));
        }
    }

    return result;
}

/**
 * @brief Creates a reusable findAll function
 */
template<typename T>
auto makeFindAll(auto&& predicate) {
    return [pred = std::forward<decltype(predicate)>(predicate)](const GenSerie<T>& serie) {
        return findAll(pred, serie);
    };
}

// ----------------------------------------------------------------

// Helpers for current cases

// Trouve les valeurs égales à une valeur donnée (scalaire)
template <typename T>
FindResult<T> find_equal(const GenSerie<T> &serie, T value) {
    return find([value](T v, uint32_t) { return v == value; }, serie);
}

// Trouve les valeurs dans un intervalle (scalaire)
template <typename T>
FindResult<T> find_range(const GenSerie<T> &serie, T min, T max) {
    return find([min, max](T v, uint32_t) { return v >= min && v <= max; },
                serie);
}

// Trouve les vecteurs dont la norme est supérieure à une valeur
template <typename T>
FindResult<T> find_norm_greater(const GenSerie<T> &serie, T threshold) {
    return find(
        [threshold](const std::vector<T> &v, uint32_t) {
            T norm = 0;
            for (const auto &x : v)
                norm += x * x;
            return std::sqrt(norm) > threshold;
        },
        serie);
}

} // namespace df
