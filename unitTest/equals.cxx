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

#include <dataframe/functional/math/equals.h>
#include <dataframe/Serie.h>

using namespace df;

int main()
{
    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 2, 3});
        bool equal = df::math::equals(s1, s2); // true
    }

    {
        Serie s1(1, {1.0, 2.0, 3.0});
        Serie s2(1, {1.0000001, 2.0, 3.0});
        bool equal = df::math::approximatelyEquals(s1, s2, 1e-6); // true
    }

    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 2, 3});
        Serie s3(1, {1, 2, 3});
        bool allEqual = df::math::equalsAll(s1, s2, s3); // true
    }

    // {
    //     Serie s1(1, {1.0, 2.0, 3.0});
    //     Serie s2(1, {1.0000001, 2.0, 3.0});
    //     Serie s3(1, {1.0000002, 2.0, 3.0});
    //     bool allEqual = approximatelyEqualsAll(s1, s2, s3); // true
    // }

    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 4, 3});
        auto mask = df::math::equalityMask(s1, s2); // Serie(1, {1, 0, 1})
    }

    {
        Serie s1(1, {1.0, 2.0, 3.0});
        Serie s2(1, {1.0000001, 4.0, 3.0});
        auto mask = df::math::approximateEqualityMask(s1, s2, 1e-6); // Serie(1, {1, 0, 1})
    }
}