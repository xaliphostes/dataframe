/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include "types.h"
#include <cstdint>
#include <iomanip>
#include <vector>

namespace df {

class SerieBase {
  public:
    virtual ~SerieBase() = default;
    virtual size_t size() const = 0;
    // virtual void printValueAt(std::ostream &, size_t, size_t, size_t) const = 0;
    virtual std::string type() const = 0;
};

template <typename T> class Serie : public SerieBase {
  public:
    using value_type = T;
    using ArrayType = std::vector<T>;
    using Self = Serie<T>;

    Serie() = default;

    Serie(const ArrayType &values);
    Serie(const std::initializer_list<T> &values);
    explicit Serie(size_t size) : data_(size) {}
    Serie(size_t size, const T &value) : data_(size, value) {}

    std::string type() const override;
    size_t size() const override;
    bool empty() const;

    T &operator[](size_t index);
    const T &operator[](size_t index) const;

    void add(const T &value) { data_.push_back(value); }

    const ArrayType &data() const;
    const ArrayType &asArray() const;

    template <typename F> void forEach(F &&callback) const;
    template <typename F> auto map(F &&callback) const;
    template <typename F, typename AccT> auto reduce(F &&, AccT) const;

    // void printValueAt(std::ostream &, size_t, size_t, size_t) const override;

  private:
    ArrayType data_;
};

} // namespace df

template <typename T>
std::ostream &operator<<(std::ostream &o, const df::Serie<T> &s);

#include "inline/Serie.hxx"
