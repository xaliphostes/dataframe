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
#include <dataframe/Serie.h>
#include <type_traits>

/**
 * @code
 * // With series values
 * Serie<bool> condition{true, false, true};
 * Serie<int> then_serie{1, 2, 3};
 * Serie<int> else_serie{10, 20, 30};
 * auto result1 = where(condition, then_serie, else_serie);  // {1, 20, 3}
 *
 * // With scalar values
 * auto result2 = where(condition, 100, -100);  // {100, -100, 100}
 *
 * // Using pipeline syntax
 * auto result3 = condition | bind_where(then_serie, else_serie);
 * auto result4 = condition | bind_where(100, -100);
 * @endcode
 */

namespace df {

    /**
     * @brief Element-wise selection based on a condition
     * The where function takes a condition Serie and two other Series (or scalar values) of the
     * same length. It returns a new Serie where each element is selected from the "then" Serie if
     * the corresponding condition is true, or from the "else" Serie if the condition is false. This
     * allows for conditional transformations of data based on a boolean mask.
     * @tparam CondT The type of the condition elements (should be convertible to bool
     * for proper evaluation)
     * @tparam ThenT The type of the "then" elements
     * @tparam ElseT The type of the "else" elements
     * @param condition A Serie of conditions that determine which value to select
     * @param then_serie A Serie containing values to select when the condition is true
     * @param else_serie A Serie containing values to select when the condition is false
     * @return A new Serie where each element is selected from then_serie or else_serie based on the
     * corresponding condition
     */
    template <typename CondT, typename ThenT, typename ElseT>
    auto where(const Serie<CondT>& condition, const Serie<ThenT>& then_serie,
        const Serie<ElseT>& else_serie);

    template <typename CondT, typename ThenT, typename ElseT>
    auto where(const Serie<CondT>& condition, const ThenT& then_value, const ElseT& else_value);

    template <typename ThenT, typename ElseT>
    auto bind_where(const Serie<ThenT>& then_serie, const Serie<ElseT>& else_serie);

    template <typename ThenT, typename ElseT>
    auto bind_where(const ThenT& then_value, const ElseT& else_value);

} // namespace df

#include "inline/where.hxx"