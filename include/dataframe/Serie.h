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
#include <dataframe/functional/common.h>

#include <functional>
#include <iostream>
#include <numeric>
#include <variant>

/** @defgroup Algebra Math-algebra functionalities
 *  @brief Algebra operations and utilities
 */

/** @defgroup Conditional Conditional functionalities
 *  @brief Conditional features
 */

/** @defgroup Geo Geo functionalities
 *  @brief Geo features, i.e., geophysics, geology, geometry...
 */

/** @defgroup Math Math functionalities
 *  @brief Math features
 */

/** @defgroup Stats Statistic functionalities
 *  @brief Stats features
 */

/** @defgroup Utils Utils functionalities
 *  @brief Utils features
 */

/** @defgroup Attributes Attributes functionalities
 *  @brief Attributes features
 */

namespace df {

template <typename T> class GenSerie {
  public:
    using value_type = T;
    using Array = std::vector<T>;

    GenSerie(int itemSize = 0, uint32_t count = 0, uint = 3);
    GenSerie(int itemSize, const Array &values, uint = 3);
    GenSerie(int itemSize, const std::initializer_list<T> &values, uint = 3);
    GenSerie(const GenSerie &s);

    static GenSerie create(int itemSize, const Array &data, uint dimension = 3);
    bool isValid() const;
    bool isEmpty() const;
    std::string type() const;
    void reCount(uint32_t c);
    GenSerie &operator=(const GenSerie &s);
    GenSerie clone() const;
    uint32_t size() const;
    uint32_t count() const;
    uint32_t itemSize() const;
    uint dimension() const;
    Array array(uint32_t i) const;
    T value(uint32_t i) const;
    void setArray(uint32_t i, const Array &v);
    void setValue(uint32_t i, T v);
    const Array &asArray() const;
    Array &asArray();

    template <typename U = Array>
    auto get(uint32_t i) const
        -> std::conditional_t<details::is_array_v<U>, Array, T>;
    template <typename U> void set(uint32_t i, const U &value);
    template <typename F> void forEach(F &&cb) const;
    template <typename F> auto map(F &&cb) const;

  private:
    Array s_;
    uint32_t count_{0};
    uint dimension_{3};
    int itemSize_{1};
};

// ==============================================================
// Define a Serie for double as we are using it a lot!
using Serie = GenSerie<double>;
// ==============================================================

template <typename T>
std::ostream &operator<<(std::ostream &o, const GenSerie<T> &s);

// ==============================================================
namespace details {

template <typename T>
using IsSerieFloating =
    std::enable_if_t<df::details::is_floating_v<T>, GenSerie<T>>;

template <typename T>
using IsSerieFunction =
    std::enable_if_t<is_floating_v<T>,
                     std::function<GenSerie<T>(const GenSerie<T> &)>>;

// Helper pour vérifier les counts
template <typename T, typename... Args>
inline bool check_counts(const GenSerie<T> &first, const Args &...args) {
    return (... && (first.count() == args.count()));
}

} // namespace details
// ==============================================================

} // namespace df

#include "inline/Serie.hxx"
