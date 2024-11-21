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

namespace df
{

    /**
     * @brief Applies a single operation to a Serie
     * @param serie The input Serie
     * @param op Operation to apply (function that takes a Serie and returns a Serie)
     * @return Serie A new Serie after applying the operation
     *
     * @example
     * ```cpp
     * Serie s(1, {1, 2, 3, 4});
     * auto result = pipe(s, [](const Serie& s) {
     *     return s.map([](double v, uint32_t) { return v * 2; });
     * });
     * ```
     */
    template <typename F>
    Serie pipe(const Serie &serie, F &&op)
    {
        return op(serie);
    }

    /**
     * @brief Applies multiple operations to a Serie in sequence
     * @param serie The input Serie
     * @param op First operation
     * @param ops Rest of the operations
     * @return Serie A new Serie after applying all operations in sequence
     *
     * @example
     * ```cpp
     * Serie s(1, {1, 2, 3, 4});
     * auto result = pipe(s,
     *     [](const Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
     *     [](const Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); },
     *     [](const Serie& s) { return s.map([](double v, uint32_t) { return v + 1; }); }
     * );
     * ```
     */
    template <typename F, typename... Fs>
    Serie pipe(const Serie &serie, F &&op, Fs &&...ops)
    {
        return pipe(op(serie), std::forward<Fs>(ops)...);
    }

    /**
     * @brief Creates a pipeline of operations that can be applied to any Serie
     * @param op First operation
     * @param ops Rest of the operations
     * @return Function that takes a Serie and returns a Serie
     *
     * @example
     * ```cpp
     * auto pipeline = makePipe(
     *     [](const Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
     *     [](const Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); }
     * );
     *
     * Serie s1(1, {1, 2, 3, 4});
     * Serie s2(1, {5, 6, 7, 8});
     *
     * auto result1 = pipeline(s1);
     * auto result2 = pipeline(s2);
     * ```
     */
    template <typename... Fs>
    auto makePipe(Fs &&...ops)
    {
        return [... ops = std::forward<Fs>(ops)](const Serie &serie)
        {
            return pipe(serie, ops...);
        };
    }
    
}
