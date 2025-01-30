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
#include <sstream>
#include <string>

/**
 * Waiting for LLVM to support the std::format functions (C++20).
 *
 * Usage:
 * @code
 * int age = 25;
 * std::string name = "Alice";
 * std::string result = format("Name: ", name, ", Age: ", age);
 * @endcode
 */
template <typename... Args> std::string format(const Args &...args) {
    std::ostringstream oss;
    (oss << ... << args);
    return oss.str();
}

// ------------------------------------------------------

/**
 * @brief Macro that allows to generate a function to be used in pipe (for
 * example).
 * @example
 * ```cpp
 * Serie scale(const Serie&, double sc);
 * MAKE_OP(scale); // will generate: bind_scale
 *
 * auto result = pipe(
 *      serie,
 *      bind_scale(2),
 *      bind_scale(10)
 * )
 * ```
 */
#define MAKE_OP(op)                                                            \
    template <typename F> auto bind_##op(F &&cb) {                             \
        return [cb = std::forward<F>(cb)](const auto &serie) {                 \
            return op(cb, serie);                                              \
        };                                                                     \
    }
