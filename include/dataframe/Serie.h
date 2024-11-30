/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <dataframe/types.h>

#include <iostream>
#include <numeric>
#include <variant>

namespace df {
  namespace detail {
    // Trait for tetecting if a type is an array
    template <typename T> struct is_array : std::false_type {};

    template <> struct is_array<Array> : std::true_type {};

    template <typename T>
    inline constexpr bool is_array_v = is_array<std::decay_t<T>>::value;

    // For array_size
    template <typename T> struct array_size {
      static constexpr size_t value = 1;
    };

    template <> struct array_size<Array> {
      static constexpr size_t value = 0; // dynamic size
    };

    // Type traits for detecting the function signature
    template <typename F, typename = void>
    struct is_scalar_callback : std::false_type {};

    template <typename F>
    struct is_scalar_callback<
        F, std::void_t<decltype(std::declval<F>()(std::declval<double>(),
                                                  std::declval<uint32_t>()))>>
        : std::true_type {};

    template <typename F>
    inline constexpr bool is_scalar_callback_v = is_scalar_callback<F>::value;

  } // namespace detail

  /**
   * @brief The most important class of this library.
   * @todo A concise description...
   * @example
   * For a Serie of non-scalar (itemSize>1):
   * ```c++
   * Serie s1(3, {1,2,3, 4,5,6});
   * s1.forEach( [](const Array &v, uint32_t) {
   *      std::cerr << v << std::endl ;
   * });
   * // Display:
   * // 1 2 3
   * // 4 5 6
   *
   * Serie s2(1, {1, 3});
   * s2.forEach( [](double v, uint32_t) {
   *      std::cerr << v << std::endl ;
   * });
   * // Display:
   * // 1
   * // 3
   * ```
   */
  class Serie {
  public:
    Serie(int itemSize = 0, uint32_t count = 0, uint dimension = 3);
    Serie(int itemSize, const Array &values, uint dimension = 3);
    Serie(int itemSize, const std::initializer_list<double> &values,
          uint dimension = 3);
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

    template <typename T = Array>
    auto get(uint32_t i) const
        -> std::conditional_t<detail::is_array_v<T>, Array, double>;

    template <typename T> void set(uint32_t i, const T &value);

    // -----------------------------------------------------

    template <typename F> void forEach(F &&cb) const;

    template <typename F> Serie map(F &&cb) const;

    template <typename F> auto reduce(F &&cb, double init);

    template <typename F> auto reduce(F &&cb, const Array &init);

    template <typename F> Serie filter(F &&reduceFn) const;

    template <typename F> Serie pipe(F &&op) const;

    template <typename F, typename... Fs> Serie pipe(F &&op, Fs &&...ops) const;

  private:
    Array s_;
    uint32_t count_{0};
    uint dimension_{3};
    int itemSize_{1};
  };

  // ---------------------------------------------

  using Series = std::vector<Serie>;

  // ---------------------------------------------

  inline Serie toSerie(uint32_t itemSize, const Array &a, uint dim = 3) {
    return Serie(itemSize, a, 3);
  }

  // ---------------------------------------------

  std::ostream &operator<<(std::ostream &o, const Serie &a);
} // namespace df

#include "inline/Serie.hxx"
