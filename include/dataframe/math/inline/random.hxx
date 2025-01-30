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

#include <chrono>
#include <random>
#include <type_traits>

namespace df {

namespace detail {

template <typename T> class RandomGenerator {
  public:
    RandomGenerator()
        : engine_(std::chrono::system_clock::now().time_since_epoch().count()) {
    }

    std::vector<T> generate(size_t n, T min, T max) {
        std::vector<T> result;
        result.reserve(n);

        if constexpr (std::is_integral_v<T>) {
            std::uniform_int_distribution<T> dist(min, max);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(dist(engine_));
            }
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(dist(engine_));
            }
        }

        return result;
    }

  private:
    std::mt19937 engine_;
};

} // namespace detail

template <typename T> inline Serie<T> random(size_t n, T min, T max) {
    detail::RandomGenerator<T> generator;
    return Serie<T>(generator.generate(n, min, max));
}

} // namespace df