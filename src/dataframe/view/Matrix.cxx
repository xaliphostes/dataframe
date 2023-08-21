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
#include <dataframe/view/Matrix.h>
#include <cmath>

namespace df
{

    uint index(uint i, uint j, uint m)
    {
        return i * m + j;
    }

    uint indexS(uint i, uint j, uint m)
    {
        if (j < i)
        {
            return 0.5 * j * (2 * m - 1 - j) + i;
        }

        return 0.5 * i * (2 * m - 1 - i) + j;
    }

    // ---------------------------------------------------------

    Matrix::Matrix(const Array &v, uint m) : v_(v), m_(m)
    {
    }

    uint32_t Matrix::length() const
    {
        return v_.size();
    }

    uint Matrix::index(uint i, uint j) const
    {
        return ::df::index(i, j, m_);
    }

    double Matrix::at(uint i, uint j) const
    {
        return v_[index(i, j)];
    }

    bool Matrix::isSymmetric() const {
        bool ok = true;
        for (uint32_t i=0; i<m_; ++i) {
            for (uint32_t j=i; j<m_; ++j) {
                ok = ok && (at(i,j) == at(j,i));
            }
        }
        return ok;
    }

    Vector Matrix::array() const
    {
        if (isSymmetric())
        {
            Array a(m_ * (m_ + 1) / 2, 0);
            uint32_t k = 0;
            for (uint32_t i = 0; i < m_; ++i)
            {
                for (uint32_t j = i; j < m_; ++j)
                {
                    a[k++] = at(i, j);
                }
            }
            return a;
        }
        return v_;
    }

    double Matrix::trace() const
    {
        double t = 0;
        for (uint32_t i = 0; i < m_; ++i)
        {
            t += at(i, i);
        }
        return t;
    }

    Matrix Matrix::transpose() const
    {
        Array v(v_.size(), 0);
        uint32_t id = 0;
        for (uint32_t j = 0; j < m_; ++j)
        {
            for (uint32_t i = 0; i < m_; ++i)
            {
                v[id++] = at(i, j);
            }
        }
        return Matrix(v, m_);
    }

    Vector Matrix::multVec(const Vector &v) const
    {
        if (v.length() != m_)
        {
            throw std::invalid_argument("size mismatch for product matrix vector");
        }

        Array t(v.length(), 0);
        const Array &a = v.array();

        for (uint32_t i = 0; i < m_; ++i)
        {
            for (uint32_t j = 0; j < m_; ++j)
            {
                t[i] += at(i, j) * a[j];
            }
        }
        return Vector(t);
    }

    Matrix Matrix::multMat(const Matrix &m) const
    {
        if (m.m_ != m_)
        {
            throw std::invalid_argument("size mismatch for product matrix matrix");
        }

        Array t(m.length(), 0);

        for (uint32_t i = 0; i < m_; ++i)
        {
            for (uint32_t j = 0; j < m_; ++j)
            {
                for (uint32_t k = 0; k < m_; ++k)
                {
                    t[index(i, j)] += at(i, k) * m.at(k, j);
                }
            }
        }

        return Matrix(t, m_);
    }

    // ---------------------------------------------

    Matrix squaredMatrix(const Array &v)
    {
        auto m = std::sqrt(v.size());
        return Matrix(v, m);
    }

    Matrix symSquaredMatrix(const Array &v)
    {
        auto m = (std::sqrt(8 * v.size() + 1) - 1) / 2;
        Array w = createArray(m * m, 0);
        for (uint i = 0; i < m; ++i)
        {
            for (uint j = 0; j < m; ++j)
            {
                w[index(j, i, m)] = v[indexS(i, j, m)];
            }
        }
        return Matrix(w, m);
    }

}
