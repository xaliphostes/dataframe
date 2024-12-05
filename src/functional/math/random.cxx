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

#include <dataframe/functional/math/random.h>

namespace df {
namespace math {

/**
 * @brief Generate a Serie with uniform random values in [min, max]
 * @param count Number of items
 * @param itemSize Size of each item (1 for scalar, >1 for vector)
 * @param min Minimum value
 * @param max Maximum value
 * @param seed Optional random seed
 * @example
 * ```cpp
 * // 100 scalars in [-1,1]
 * auto scalar_series = random(100, 1, -1.0, 1.0);
 *
 * // 50 3D vectors with components in [0,10]
 * auto vector_series = random(50, 3, 0.0, 10.0);
 * ```
 */
Serie random(uint32_t count, uint32_t itemSize, double min, double max,
             unsigned seed) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis(min, max);

    Serie result(itemSize, count);

    for (uint32_t i = 0; i < count; ++i) {
        if (itemSize == 1) {
            result.set(i, dis(gen));
        } else {
            Array values(itemSize);
            std::generate(values.begin(), values.end(),
                          [&]() { return dis(gen); });
            result.set(i, values);
        }
    }

    return result;
}

} // namespace math
} // namespace df
