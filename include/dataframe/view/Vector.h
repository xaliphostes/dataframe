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
#include <dataframe/types.h>

namespace df
{

    class Vector
    {
    public:
        Vector();
        Vector(const Array &v);
        double operator[](uint32_t i) const;
        double at(uint32_t i) const;
        uint32_t length() const;
        const Array &array() const;
        Vector &normalize();
        double norm() const;
        Vector &mult(double s);
        Vector &mult(const Vector &s);
        Vector &add(const Vector &s);
        Vector &sub(const Vector &s);
        double dot(const Vector &s) const;
        String toString() const;

    private:
        Array v_;
    };

}
