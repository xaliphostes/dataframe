/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <algorithm>
#include <cmath>
#include <dataframe/Serie.h>
#include <limits>

namespace df {
namespace math {

constexpr double DEFAULT_EPSILON = std::numeric_limits<double>::epsilon() * 100;

inline bool equals(const Serie &s1, const Serie &s2) {
    // Check basic properties
    if (!s1.isValid() || !s2.isValid())
        return false;
    if (s1.itemSize() != s2.itemSize())
        return false;
    if (s1.count() != s2.count())
        return false;
    if (s1.dimension() != s2.dimension())
        return false;

    // Compare actual values
    return s1.asArray() == s2.asArray();
}

MAKE_OP(equals);

inline bool approximatelyEquals(const Serie &s1, const Serie &s2,
                                double epsilon = DEFAULT_EPSILON) {
    // Check basic properties
    if (!s1.isValid() || !s2.isValid())
        return false;
    if (s1.itemSize() != s2.itemSize())
        return false;
    if (s1.count() != s2.count())
        return false;
    if (s1.dimension() != s2.dimension())
        return false;

    const auto &arr1 = s1.asArray();
    const auto &arr2 = s2.asArray();

    return std::equal(
        arr1.begin(), arr1.end(), arr2.begin(),
        [epsilon](double a, double b) { return std::abs(a - b) <= epsilon; });
}

MAKE_OP(approximatelyEquals);

/**
 * @brief Base case for equalsAll recursion
 */
inline bool equalsAll(const Serie &serie) { return true; }

/**
 * @brief Recursive case for equalsAll
 */
template <typename... Series>
bool equalsAll(const Serie &first, const Serie &second, const Series &...rest) {
    if (!equals(first, second))
        return false;
    return equalsAll(second, rest...);
}

/**
 * @brief Base case for approximatelyEqualsAll recursion
 */
inline bool approximatelyEqualsAll(const Serie &serie,
                                   double epsilon = DEFAULT_EPSILON) {
    return true;
}

/**
 * @brief Recursive case for approximatelyEqualsAll
 */
template <typename... Series>
bool approximatelyEqualsAll(const Serie &first, const Serie &second,
                            const Series &...rest,
                            double epsilon = DEFAULT_EPSILON) {
    if (!approximatelyEquals(first, second, epsilon))
        return false;
    return approximatelyEqualsAll(second, rest..., epsilon);
}

/**
 * @brief Creates a Serie indicating where two Series are equal
 * @param s1 First Serie
 * @param s2 Second Serie
 * @return Serie A new Serie with 1.0 where values are equal, 0.0 otherwise
 *
 * @example
 * ```cpp
 * Serie s1(1, {1, 2, 3});
 * Serie s2(1, {1, 4, 3});
 * auto diff = equalityMask(s1, s2); // Serie(1, {1, 0, 1})
 * ```
 */
inline Serie equalityMask(const Serie &s1, const Serie &s2) {
    if (!s1.isValid() || !s2.isValid() || s1.itemSize() != s2.itemSize() ||
        s1.count() != s2.count()) {
        throw std::invalid_argument("Series must have same dimensions");
    }

    Serie result(1, s1.count());
    for (uint32_t i = 0; i < s1.count(); ++i) {
        if (s1.itemSize() == 1) {
            result.setScalar(i, s1.scalar(i) == s2.scalar(i) ? 1.0 : 0.0);
        } else {
            Array v1 = s1.value(i);
            Array v2 = s2.value(i);
            bool equal = std::equal(v1.begin(), v1.end(), v2.begin());
            result.setScalar(i, equal ? 1.0 : 0.0);
        }
    }
    return result;
}

/**
 * @brief Creates a Serie indicating where two Series are approximately equal
 * @param s1 First Serie
 * @param s2 Second Serie
 * @param epsilon Maximum allowed difference between values
 * @return Serie A new Serie with 1.0 where values are approximately equal, 0.0
 * otherwise
 *
 * @example
 * ```cpp
 * Serie s1(1, {1.0, 2.0, 3.0});
 * Serie s2(1, {1.0000001, 4.0, 3.0});
 * auto diff = approximateEqualityMask(s1, s2, 1e-6); // Serie(1, {1, 0, 1})
 * ```
 */
inline Serie approximateEqualityMask(const Serie &s1, const Serie &s2,
                                     double epsilon = DEFAULT_EPSILON) {
    if (!s1.isValid() || !s2.isValid() || s1.itemSize() != s2.itemSize() ||
        s1.count() != s2.count()) {
        throw std::invalid_argument("Series must have same dimensions");
    }

    Serie result(1, s1.count());
    for (uint32_t i = 0; i < s1.count(); ++i) {
        if (s1.itemSize() == 1) {
            result.setScalar(i, std::abs(s1.scalar(i) - s2.scalar(i)) <= epsilon
                                    ? 1.0
                                    : 0.0);
        } else {
            Array v1 = s1.value(i);
            Array v2 = s2.value(i);
            bool approxEqual = std::equal(v1.begin(), v1.end(), v2.begin(),
                                          [epsilon](double a, double b) {
                                              return std::abs(a - b) <= epsilon;
                                          });
            result.setScalar(i, approxEqual ? 1.0 : 0.0);
        }
    }
    return result;
}

} // namespace math
} // namespace df