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
    virtual std::string type() const = 0;
};

// --------------------------------------------------------------

/**
 * @brief A Serie is a column of data. It is a vector of values of type T.
 * @tparam T The type of the data in the Serie.
 */
template <typename T> class Serie : public SerieBase {
  public:
    using value_type = T;
    using ArrayType = std::vector<T>;
    using Self = Serie<T>;
    using iterator = typename ArrayType::iterator;
    using const_iterator = typename ArrayType::const_iterator;

    /**
     * @brief Get iterator to beginning of the Serie
     */
    iterator begin() { return data_.begin(); }

    /**
     * @brief Get const iterator to beginning of the Serie
     */
    const_iterator begin() const { return data_.begin(); }

    /**
     * @brief Get const iterator to beginning of the Serie
     */
    const_iterator cbegin() const { return data_.cbegin(); }

    /**
     * @brief Get iterator to end of the Serie
     */
    iterator end() { return data_.end(); }

    /**
     * @brief Get const iterator to end of the Serie
     */
    const_iterator end() const { return data_.end(); }

    /**
     * @brief Get const iterator to end of the Serie
     */
    const_iterator cend() const { return data_.cend(); }

    Serie() = default;

    /**
     * @brief Construct a Serie from a vector of values.
     */
    Serie(const ArrayType &values);

    /**
     * @brief Construct a Serie from an initializer list of values.
     */
    Serie(const std::initializer_list<T> &values);

    /**
     * @brief Construct a Serie of a given size.
     */
    explicit Serie(size_t size) : data_(size) {}

    /**
     * @brief Construct a Serie of a given size, filled with a given value.
     */
    Serie(size_t size, const T &value) : data_(size, value) {}

    /**
     * @brief Get the type of the Serie as a string.
     */
    std::string type() const override;

    /**
     * @brief Get the size of the Serie.
     */
    size_t size() const override;

    /**
     * @brief Check if the Serie is empty.
     */
    bool empty() const;

    /**
     * @brief Convert Serie to a different type
     * @tparam U Target type to convert to
     * @return A new Serie with converted values
     */
    template <typename U> Serie<U> as() const;

    /**
     * @brief Reserve space for future elements.
     * @param n The number of elements to reserve space for
     */
    void reserve(size_t n) { data_.reserve(n); }

    /**
     * @brief Get the value at a given index.
     */
    T &operator[](size_t index);

    /**
     * @brief Get the value at a given index.
     */
    const T &operator[](size_t index) const;

    /**
     * @brief Add a value to the Serie.
     */
    void add(const T &value) { data_.push_back(value); }

    /**
     * @brief Get the data of the Serie.
     */
    const ArrayType &data() const;

    /**
     * @brief Get the data of the Serie.
     * @see data()
     */
    const ArrayType &asArray() const;

    /**
     * @brief Apply a function to each element of the Serie.
     */
    template <typename F> void forEach(F &&callback) const;

    /**
     * @brief Apply a function to each element of the Serie and return a new
     * Serie with the results.
     */
    template <typename F> auto map(F &&callback) const;

    /**
     * @brief Filter the elements of the Serie using a predicate.
     */
    template <typename F, typename AccT> auto reduce(F &&, AccT) const;

  private:
    ArrayType data_;
};

} // namespace df

template <typename T>
std::ostream &operator<<(std::ostream &o, const df::Serie<T> &s);

#include "inline/Serie.hxx"
