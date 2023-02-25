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

class Serie
{
public:
    Serie(int itemSize = 0, uint32_t count = 0);
    Serie(int itemSize, const Array &values);
    Serie(const Serie &s);
    Serie &operator=(const Serie &s);

    uint32_t size() const;
    uint32_t count() const;
    uint32_t itemSize() const;

    void dump() const;
    Array itemAt(uint32_t i) const;
    const Array &array() const;
    Array &array();
    template <typename F>
    void forEach(F &&cb) const;
    template <typename F>
    Serie map(F &&cb) const;
    template <typename F>
    Array reduce(F &&reduceFn, const Array &acc);

private:
    Array s_;
    uint32_t count_{0};
    int itemSize_{1};
};

#include "Serie.hxx"
