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

#include <cmath>
#include <dataframe/functional/math/mult.h>
#include <dataframe/functional/math/sub.h>
#include <dataframe/functional/stats/covariance.h>
#include <dataframe/functional/stats/mean.h>
#include <dataframe/types.h>

namespace df {
namespace stats {

double covariance(const Serie &x, const Serie &y) {
    if (x.count() != y.count()) {
        throw std::invalid_argument(
            "covariance: x and y must have the same length");
    }
    if (x.itemSize() != 1) {
        throw std::invalid_argument("covariance: x must have itemSize = 1");
    }
    if (y.itemSize() != 1) {
        throw std::invalid_argument("covariance: y must have itemSize = 1");
    }

    double N = x.size();
    double xb = mean(x).number;
    double yb = mean(y).number;

    return math::mult(math::sub(x, xb), math::sub(y, yb))
        .reduce(
            [N](double acc, double value, uint32_t) { return acc + value / N; },
            0);
}

} // namespace stats
} // namespace df
