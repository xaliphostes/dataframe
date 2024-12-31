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
#include <dataframe/view/Vector.h>
#include <cmath>
#include <numeric>
#include <algorithm>

namespace df
{

    Vector::Vector()
    {
    }

    Vector::Vector(const Array &v) : v_(v)
    {
    }

    double Vector::operator[](uint32_t i) const
    {
        return v_[i];
    }

    double Vector::at(uint32_t i) const
    {
        return v_[i];
    }

    uint32_t Vector::length() const
    {
        return v_.size();
    }

    const Array &Vector::array() const
    {
        return v_;
    }

    Vector &Vector::normalize()
    {
        auto n = norm();
        auto op = [n](double x)
        { return x / n; };
        std::transform(v_.begin(), v_.end(), v_.begin(), op);
        return *this;
    }

    double Vector::norm() const
    {
        auto op = [](double acc, double v)
        { return acc + v * v; };
        return std::sqrt(std::accumulate(v_.begin(), v_.end(), 0, op));
    }

    Vector &Vector::mult(double s)
    {
        auto op = [s](double x)
        { return x * s; };
        std::transform(v_.begin(), v_.end(), v_.begin(), op);
        return *this;
    }

    Vector &Vector::mult(const Vector &s)
    {
        uint32_t id = 0;
        auto op = [s, id](double x) mutable
        { return x * s[id++]; };
        std::transform(v_.begin(), v_.end(), v_.begin(), op);
        return *this;
    }

    Vector &Vector::add(const Vector &s)
    {
        uint32_t id = 0;
        auto op = [s, id](double x) mutable
        { return x + s[id++]; };
        std::transform(v_.begin(), v_.end(), v_.begin(), op);
        return *this;
    }

    Vector &Vector::sub(const Vector &s)
    {
        uint32_t id = 0;
        auto op = [s, id](double x) mutable
        { return x - s[id++]; };
        std::transform(v_.begin(), v_.end(), v_.begin(), op);
        return *this;
    }

    double Vector::dot(const Vector &s) const
    {
        double d = 0;
        for (uint32_t i = 0; i < v_.size(); ++i)
        {
            d += v_[i] * s.v_[i];
        }
        return d;
    }

    String Vector::toString() const
    {
        String s;
        for (uint32_t i = 0; i < v_.size(); ++i)
        {
            s += std::to_string(at(i)) + "\t";
        }
        return s;
    }

}
