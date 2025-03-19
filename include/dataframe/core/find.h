/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>
#include <optional>

namespace df {

/**
 * # Single series
 * - series | df::bind_find_all(pred);                    // All matches
 * - series | df::bind_find_all(pred, 3);                 // Limited matches
 * - series | df::bind_find_all_with_index(pred);         // All matches with
 * indices
 * - series | df::bind_find_all_with_index(pred, 3);      // Limited matches
 * with indices
 *
 * # Multi series
 * - series1 | df::bind_find_all(pred, series2);          // All matches across
 * - series series1 | df::bind_find_all_with_max(pred, series2, 3); // Limited
 * matches across series
 */

// Single series version - returns optional value
template <typename F, typename T>
auto find(F &&predicate, const Serie<T> &serie) -> std::optional<T>;

// Single series version with index - returns optional tuple of value and index
template <typename F, typename T>
auto find_with_index(F &&predicate, const Serie<T> &serie)
    -> std::optional<std::tuple<T, size_t>>;

// Multi series version
template <typename F, typename T, typename... Args>
auto find(F &&predicate, const Serie<T> &first, const Serie<T> &second,
          const Args &...args) -> std::optional<T>;

// Multi series version with index
template <typename F, typename T, typename... Args>
auto find_with_index(F &&predicate, const Serie<T> &first,
                     const Serie<T> &second, const Args &...args)
    -> std::optional<std::tuple<T, size_t>>;

// Bind function for multiple series
template <typename F> auto bind_find(F &&predicate, const auto &second);

template <typename F>
auto bind_find_with_index(F &&predicate, const auto &second);

MAKE_OP(find);
MAKE_OP(find_with_index);

// Find multiple values - returns Serie with specified maximum number of
// matches
template <typename F, typename T>
auto find_all(F &&predicate, const Serie<T> &serie,
              std::optional<size_t> max_matches = std::nullopt) -> Serie<T>;

// Find multiple values with indices
template <typename F, typename T>
auto find_all_with_index(F &&predicate, const Serie<T> &serie,
                         std::optional<size_t> max_matches = std::nullopt)
    -> Serie<std::tuple<T, size_t>>;

// Multi series version of find_all
template <typename F, typename T, typename... Args>
auto find_all(F &&predicate, const Serie<T> &first, const Serie<T> &second,
              const Args &...args,
              std::optional<size_t> max_matches = std::nullopt) -> Serie<T>;

// Bind functions for find_all
// Single series bind
template <typename F>
auto bind_find_all(F &&predicate,
                   std::optional<size_t> max_matches = std::nullopt);

// Multi series bind
template <typename F> auto bind_find_all(F &&predicate, const auto &second);

template <typename F>
auto bind_find_all_with_max(F &&predicate, const auto &second,
                            std::optional<size_t> max_matches);

template <typename F>
auto bind_find_all_with_index(F &&predicate,
                              std::optional<size_t> max_matches = std::nullopt);

// MAKE_OP(find_all);
// MAKE_OP(find_all_with_index);

} // namespace df

#include "inline/find.hxx"