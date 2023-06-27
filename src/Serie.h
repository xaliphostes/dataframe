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
#include "types.h"
#include <iostream>
#include <numeric>

namespace df
{

    class Serie
    {
    public:
        Serie(int itemSize = 0, uint32_t count = 0, uint dimension=3);
        Serie(int itemSize, const Array &values, uint dimension=3);
        Serie(int itemSize, const std::initializer_list<double> &values, uint dimension=3);
        Serie(const Serie &s);

        bool isValid() const;
        void reCount(uint32_t);

        Serie &operator=(const Serie &s);
        Serie clone() const;

        uint32_t size() const;
        uint32_t count() const;
        uint32_t itemSize() const;
        uint dimension() const;

        void dump() const;

        Array value(uint32_t i) const;
        double scalar(uint32_t i) const;
        void setValue(uint32_t i, const Array &v);
        void setScalar(uint32_t i, double);

        const Array &asArray() const;
        Array &asArray();

        template <typename F>
        void forEach(F &&cb) const;

        template <typename F>
        void forEachScalar(F &&cb) const;
        
        template <typename F>
        Serie map(F &&cb) const;
        
        template <typename F>
        Array reduce(F &&reduceFn, const Array &acc);

    private:
        Array s_;
        uint32_t count_{0};
        uint dimension_{3};
        int itemSize_{1};
    };

    using Series = std::vector<Serie>;

}

#include "Serie.hxx"
