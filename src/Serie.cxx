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

#include "Serie.h"
#include <iostream>

namespace df
{

    Serie::Serie(int itemSize, uint32_t count) : itemSize_(itemSize), count_(count)
    {
        s_ = Array(count * itemSize);
    }

    Serie::Serie(const Serie &s) : itemSize_(s.itemSize_), count_(s.count_)
    {
        s_ = Array(s.s_.cbegin(), s.s_.cend());
    }

    Serie::Serie(int itemSize, const Array &values) : itemSize_(itemSize)
    {
        count_ = values.size() / itemSize;
        s_ = values;
    }

    void Serie::reCount(uint32_t c)
    {
        count_ = c;
        s_ = Array(c * itemSize_);
    }

    Serie &Serie::operator=(const Serie &s)
    {
        count_ = s.count_;
        itemSize_ = s.itemSize_;
        s_ = Array(s.s_.cbegin(), s.s_.cend());
        return *this;
    }

    uint32_t Serie::size() const { return count_ * itemSize_; }

    uint32_t Serie::count() const { return count_; }

    uint32_t Serie::itemSize() const { return itemSize_; }

    void Serie::dump() const
    {
        if (itemSize_ == 1)
        {
            // forEach([](double t, uint32_t i)
            // {
            //     std::cerr << i << ": " << t << " " << std::endl;
            // });
        }
        else
        {
            forEach([](const Array &t, uint32_t i)
            {
                std::cerr << i << ": ";
                for (auto tt : t) {
                    std::cerr << tt << " ";
                }
                std::cerr << std::endl;
            });
        }

        std::cerr << std::endl;
    }

    Array Serie::value(uint32_t i) const
    {
        auto start = i * itemSize_;
        Array r(itemSize_);
        for (uint j = 0; j < itemSize_; ++j)
        {
            r[j] = s_[start + j];
        }
        return r;
    }

    double Serie::scalar(uint32_t i) const
    {
        if (i > size())
        {
            throw std::invalid_argument("index out of bounds ("+std::to_string(i) + ">=" + std::to_string(size()));
        }
        return s_[i];
    }

    void Serie::setValue(uint32_t i, const Array &v)
    {
        if (i >= count_)
        {
            throw std::invalid_argument("index out of range (" + std::to_string(i) + ">=" + std::to_string(count_) + ")");
        }

        auto size = itemSize_;
        if (v.size() != size)
        {
            throw std::invalid_argument("provided item size (" + std::to_string(v.size()) + ") is different from itemSize (" + std::to_string(itemSize_) + ")");
        }
        for (int j = 0; j < size; ++j)
        {
            s_[i * size + j] = v[j];
        }
    }

}
