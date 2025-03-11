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

#include <dataframe/utils/utils.h>
#include <functional>

namespace df {

template <typename T>
inline Serie<T> for_loop(int start, int end, int step,
                         std::function<T(int)> callback) {
    if (step == 0) {
        throw std::invalid_argument("Step size cannot be zero");
    }

    // Calculate the number of iterations
    int n = 0;
    if (step > 0 && start < end) {
        n = (end - start + step - 1) / step;
    } else if (step < 0 && start > end) {
        n = (start - end - step - 1) / (-step);
    } else if (start == end) {
        return Serie<T>();
    } else {
        throw std::invalid_argument(
            "Invalid loop bounds with current step direction");
    }

    // Pre-allocate the result vector
    std::vector<T> result;
    result.reserve(n);

    // Execute the loop
    for (int i = start; step > 0 ? i < end : i > end; i += step) {
        result.push_back(callback(i));
    }

    return Serie<T>(result);
}

template <typename T> inline auto bind_for_loop(int start, int end, int step) {
    return [start, end, step](std::function<T(int)> callback) {
        return for_loop<T>(start, end, step, callback);
    };
}

} // namespace df