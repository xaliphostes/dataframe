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
#include <tuple>

namespace df {

/**
 * @brief Result of a find operation, containing both value and index
 */
template <typename T> struct FindResult {
    T value;        // The found value (double or Array)
    uint32_t index; // Index in the Serie
    bool found;     // Whether a value was found

    // Conversion to bool for simple tests
    operator bool() const { return found; }
};

/**
 * @brief Finds first element matching a predicate
 */
template <typename F> auto find(F &&predicate, const Serie &serie) {
    if constexpr (details::is_scalar_callback_v<F>) {
        // Scalar version
        if (serie.itemSize() != 1) {
            throw std::invalid_argument(
                "Cannot use scalar callback for Serie with itemSize > 1");
        }
        for (uint32_t i = 0; i < serie.count(); ++i) {
            double value = serie.template get<double>(i);
            if (predicate(value, i)) {
                return FindResult<double>{value, i, true};
            }
        }
        return FindResult<double>{0.0, 0, false};
    } else {
        // Vector version
        if (serie.itemSize() == 1) {
            throw std::invalid_argument(
                "Cannot use vector callback for Serie with itemSize == 1");
        }
        for (uint32_t i = 0; i < serie.count(); ++i) {
            Array value = serie.template get<Array>(i);
            if (predicate(value, i)) {
                return FindResult<Array>{value, i, true};
            }
        }
        return FindResult<Array>{Array(), 0, false};
    }
}

/**
 * @brief Creates a reusable find function
 */
template <typename F> auto makeFind(F &&predicate) {
    return [pred = std::forward<F>(predicate)](const Serie &serie) {
        return find(pred, serie);
    };
}

// --------------------------------------------------------------

/**
 * @brief Finds all elements matching a predicate
 * @return Serie containing only matching elements
 */
template <typename F> Serie findAll(F &&predicate, const Serie &serie) {
    std::vector<uint32_t> indices;

    if constexpr (details::is_scalar_callback_v<F>) {
        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (predicate(serie.template get<double>(i), i)) {
                indices.push_back(i);
            }
        }
    } else {
        for (uint32_t i = 0; i < serie.count(); ++i) {
            if (predicate(serie.template get<Array>(i), i)) {
                indices.push_back(i);
            }
        }
    }

    // Create result Serie
    Serie result(serie.itemSize(), indices.size());
    for (uint32_t i = 0; i < indices.size(); ++i) {
        if (serie.itemSize() == 1) {
            result.template set(i, serie.template get<double>(indices[i]));
        } else {
            result.template set(i, serie.template get<Array>(indices[i]));
        }
    }

    return result;
}

/**
 * @brief Creates a reusable find function
 */
template<typename F>
auto makeFindAll(F&& predicate) {
    return [pred = std::forward<F>(predicate)](const Serie& serie) {
        return findAll(pred, serie);
    };
}

} // namespace df
