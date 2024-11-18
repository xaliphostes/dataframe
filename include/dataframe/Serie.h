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
#include <iostream>
#include <numeric>

namespace df
{

    /**
     * @note For basic operations such as `forEach`, `map`, `filter` and `reduce`,
     * we provide (for optimization purpose), a **scalar** version (`forEachScalar`,
     * `mapScalar`, `filterScalar` and `reduceScalar`).
     *
     * @example
     * For a Serie of non-scalar (itemSize>1):
     * ```c++
     * Serie s(3, {1,2,3, 4,5,6});
     * s.forEach( [](const Array &v, uint32_t) {
     *      std::cerr << v << std::endl ;
     * });
     * // Display:
     * // 1 2 3
     * // 4 5 6
     * ```
     *
     * @example
     * For a Serie of scalars:
     * ```c++
     * Serie s(1, {1, 3});
     * s.forEachScalar( [](double v, uint32_t) {
     *      std::cerr << v << std::endl ;
     * });
     * // Display:
     * // 1
     * // 4
     * ```
     */
    class Serie
    {
    public:
        Serie(int itemSize = 0, uint32_t count = 0, uint dimension = 3);
        Serie(int itemSize, const Array &values, uint dimension = 3);
        Serie(int itemSize, const std::initializer_list<double> &values, uint dimension = 3);
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

        /*
         * For the following methods, I need HELP to deal with
         * scalar values and non-scalar, and in order to
         * reduce the number of methods as well as the
         * complexity of this class.
         * Maybe a templated class is possible ??!
         */

        /**
         * @param cb A callback function with signature `(const Array& current, uint32_t index)`
         */
        template <typename F>
        void forEach(F &&cb) const;

        /**
         * @param cb A callback function with signature `(double current, uint32_t index)`
         */
        template <typename F>
        void forEachScalar(F &&cb) const;

        /**
         * @param cb A callback function with signature `(const Array& current, uint32_t index)`
         */
        template <typename F>
        Serie map(F &&cb) const;

        /**
         * @param cb A callback function with signature `(double current, uint32_t index)`
         */
        template <typename F>
        Serie mapScalar(F &&cb) const;

        /**
         * @param cb A callback function with signature `(const Array& previous, const Array& current, uint32_t index)`
         */
        template <typename F>
        Serie reduce(F &&reduceFn, const Array &init) const;

        /**
         * @param cb A callback function with signature `(double previous, double current, uint32_t index)`
         */
        template <typename F>
        double reduceScalar(F &&reduceFn, double init) const;

        /**
         * @param cb A callback function with signature `(const Array& current, uint32_t index)`
         */
        template <typename F>
        Serie filter(F &&reduceFn) const;

        /**
         * @param cb A callback function with signature `(double current, uint32_t index)`
         */
        template <typename F>
        Serie filterScalar(F &&reduceFn) const;

        template <typename F>
        Serie pipe(F &&op) const ;

        template <typename F, typename... Fs>
        Serie pipe(F &&op, Fs &&...ops) const ;

    private:
        Array s_;
        uint32_t count_{0};
        uint dimension_{3};
        int itemSize_{1};
    };

    using Series = std::vector<Serie>;

    std::ostream &operator<<(std::ostream &o, const Serie &a);

}

#include "inline/Serie.hxx"
