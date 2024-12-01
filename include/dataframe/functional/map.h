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
#include <dataframe/Serie.h>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace df {

/**
 * @brief Maps a function over a Serie, allowing change in itemSize
 */
template <typename F> auto __map__(F &&cb, const Serie &serie) {
    if constexpr (details::has_scalar_input_v<F>) {
        // Input is scalar
        using ResultType = std::invoke_result_t<F, double, uint32_t>;

        if constexpr (std::is_arithmetic_v<ResultType>) {
            // Scalar to scalar (including int, double, etc.)
            Array results;
            results.reserve(serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i) {
                results.push_back(
                    static_cast<double>(cb(serie.template get<double>(i), i)));
            }
            return Serie(1, results);
        } else {
            // Scalar to vector
            if (serie.count() == 0) {
                return Serie(0, 0);
            }
            auto first_result = cb(serie.template get<double>(0), 0);
            size_t itemSize = first_result.size();

            Array flattened;
            flattened.reserve(itemSize * serie.count());
            flattened.insert(flattened.end(), first_result.begin(),
                             first_result.end());

            for (uint32_t i = 1; i < serie.count(); ++i) {
                auto result = cb(serie.template get<double>(i), i);
                if (result.size() != itemSize) {
                    throw std::runtime_error(
                        "Inconsistent output vector sizes in map");
                }
                flattened.insert(flattened.end(), result.begin(), result.end());
            }
            return Serie(itemSize, flattened);
        }
    } else {
        // Input is vector
        using ResultType = std::invoke_result_t<F, const Array &, uint32_t>;

        if constexpr (std::is_same_v<ResultType, double>) {
            // Vector to scalar
            Array results;
            results.reserve(serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i) {
                results.push_back(cb(serie.template get<Array>(i), i));
            }
            return Serie(1, results);
        } else {
            // Vector to vector
            if (serie.count() == 0) {
                return Serie(0, 0);
            }
            auto first_result = cb(serie.template get<Array>(0), 0);
            size_t itemSize = first_result.size();

            Array flattened;
            flattened.reserve(itemSize * serie.count());
            flattened.insert(flattened.end(), first_result.begin(),
                             first_result.end());

            for (uint32_t i = 1; i < serie.count(); ++i) {
                auto result = cb(serie.template get<Array>(i), i);
                if (result.size() != itemSize) {
                    throw std::runtime_error(
                        "Inconsistent output vector sizes in map");
                }
                flattened.insert(flattened.end(), result.begin(), result.end());
            }
            return Serie(itemSize, flattened);
        }
    }
}

/**
 * @brief Maps a function over a Serie, allowing change in itemSize
 */
template <typename F, typename... Args>
auto map(F &&callback, const Args &...args) {
    if constexpr (details::is_multi_series_call<Args...>::value) {
        // Multiple Series case
        static_assert(std::conjunction<details::is_serie<Args>...>::value,
                      "All arguments after callback must be Series");

        // Check counts match
        std::array<size_t, sizeof...(args)> counts = {
            details::get_count(args)...};
        for (size_t i = 1; i < counts.size(); ++i) {
            if (counts[i] != counts[0]) {
                throw std::invalid_argument(
                    "All Series must have the same count");
            }
        }

        // Get first result to determine output Serie properties
        auto first_result = callback(args.template get<Array>(0)..., 0);
        Serie result;

        if constexpr (std::is_arithmetic_v<decltype(first_result)>) {
            // Callback returns scalar
            Array results;
            results.reserve(counts[0]);
            results.push_back(first_result);

            for (uint32_t i = 1; i < counts[0]; ++i) {
                results.push_back(callback(args.template get<Array>(i)..., i));
            }
            result = Serie(1, results);
        } else {
            // Callback returns Array
            const size_t itemSize = first_result.size();
            Array flattened;
            flattened.reserve(itemSize * counts[0]);
            flattened.insert(flattened.end(), first_result.begin(),
                             first_result.end());

            for (uint32_t i = 1; i < counts[0]; ++i) {
                auto result_i = callback(args.template get<Array>(i)..., i);
                if (result_i.size() != itemSize) {
                    throw std::runtime_error(
                        "Inconsistent output vector sizes in map");
                }
                flattened.insert(flattened.end(), result_i.begin(),
                                 result_i.end());
            }
            result = Serie(itemSize, flattened);
        }
        return result;

    } else {
        // Single Serie case (original map implementation)
        static_assert(sizeof...(args) == 1,
                      "Single Serie map requires exactly one Serie");
        const auto &serie = std::get<0>(std::forward_as_tuple(args...));
        return __map__(callback, serie);
    }
}

/**
 * @brief Creates a reusable map function
 */
MAKE_OP(map);

} // namespace df
