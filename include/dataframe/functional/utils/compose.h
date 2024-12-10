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

namespace df {
namespace utils {

// Helper type to deduce return type of composed functions
template <typename F, typename G> struct ComposedType {
    template <typename... Args> auto operator()(Args &&...args) const {
        return f(g(std::forward<Args>(args)...));
    }
    F f;
    G g;
};

// Binary compose
template <typename F, typename G> auto compose(F &&f, G &&g) {
    return ComposedType<std::decay_t<F>, std::decay_t<G>>{std::forward<F>(f),
                                                          std::forward<G>(g)};
}

// Variadic compose
/**
 * @example
 * ```cpp
 * auto f = [](double x) { return x * 2; };
 * auto g = [](double x) { return x + 1; };
 * auto h = [](double x) { return x * x; };
 *
 * auto composed = df::compose(f, g, h);
 * auto result = composed(3.0); // f(g(h(3.0)))
 * ```
 */
template <typename F, typename G, typename... Fs>
auto compose(F &&f, G &&g, Fs &&...fs) {
    return compose(std::forward<F>(f),
                   compose(std::forward<G>(g), std::forward<Fs>(fs)...));
}

} // namespace utils
} // namespace df
