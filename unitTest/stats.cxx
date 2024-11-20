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

#include <iostream>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/functional/stats/bins.h>
#include <dataframe/functional/stats/mean.h>
#include <dataframe/functional/stats/covariance.h>
#include "assertions.h"

START_TEST(bins) {
    df::Serie s(1, {57, 57, 57, 58, 63, 66, 66, 67, 67, 68, 69, 70, 70, 70, 70, 72, 73, 75, 75, 76, 76, 78, 79, 81});

    {
        df::Serie a = df::bins(s, 10);

        uint32_t nb1 = s.count();
        Array sol{4, 0, 1, 2, 3, 5, 2, 4, 1, 2};
        assertArrayEqual(a.asArray(), sol);

        auto nb2 = std::reduce(sol.begin(), sol.end());
        assertEqual(nb1, (uint32_t)nb2);
    }

    {
        df::Serie a = df::bins(s, 10, 0, 100);
        uint nb1 = s.count();
        Array sol{0, 0, 0, 0, 0, 4, 7, 12, 1, 0};
        assertArrayEqual(a.asArray(), sol);

        auto nb2 = std::reduce(sol.begin(), sol.end());
        assertEqual(nb1, (uint32_t)nb2);
    }
} END_TEST(bins)

START_TEST(mean1) {
    df::Serie serie(1, {0,1,2,3,4,5,6,7,8});
    double a = df::mean(serie).number;
    double sol = (9 * (9 - 1)) / 2 / 9;
    assertDoubleEqual(a, sol);
} END_TEST(mean1)

START_TEST(mean3) {
    df::Serie serie(3, {0,1,2, 3,4,5, 6,7,8});
    Array a = df::mean(serie).array;
    Array sol{3, 4, 5};
    assertArrayEqual(a, sol);
} END_TEST(mean3)

START_TEST(cov) {
    df::Serie x, y;
    double cov, mx, my;

    // mean x  : 2.5
    // mean y  : 6.5
    // cov(x,y): 1.25
    x = df::Serie(1, {1, 2, 3, 4});
    y = df::Serie(1, {5, 6, 7, 8});
    mx = df::mean(x).number;
    my = df::mean(y).number;
    cov = df::covariance(x, y);
    assertDoubleEqual(mx, 2.5);
    assertDoubleEqual(my, 6.5);
    assertDoubleEqual(cov, 1.25);

    // mean x  : 0.5518
    // mean y  : 0.302
    // cov(x,y): -0.0112
    x = df::Serie(1, {0.90010907, 0.13484424, 0.62036035});
    y = df::Serie(1, {0.12528585, 0.26962463, 0.51111198});
    mx = df::mean(x).number;
    my = df::mean(y).number;
    cov = df::covariance(x, y);
    assertDoubleEqual(mx, 0.5518, 1e-3);
    assertDoubleEqual(my, 0.302, 1e-3);
    assertDoubleEqual(cov, -0.011238, 1e-3);
} END_TEST(cov)

int main()
{
    bins();
    mean1();
    mean3();
    cov();

    return 0;
}
