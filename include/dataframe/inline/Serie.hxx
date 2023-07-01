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

#include "../utils/inferring.h"

namespace df
{

    template <typename F>
    inline void Serie::forEach(F &&cb) const
    {
        for (uint32_t i = 0; i < count_; ++i)
        {
            cb(value(i), i);
        }
    }

    template <typename F>
    inline void Serie::forEachScalar(F &&cb) const
    {
        if (itemSize_ != 1) {
            throw std::invalid_argument("(forEachScalar) itemSize is not 1 (got "+std::to_string(itemSize_)+")") ;
        }

        for (uint32_t i = 0; i < count_; ++i)
        {
            cb(scalar(i), i);
        }
    }

    template <typename F>
    Serie Serie::reduce(F &&reduceFn, const Array &acc) const
    {
        Array r = acc;
        uint itemSize = 1;
        this->forEach([&](const Array &a, uint32_t) {
            r = reduceFn(r, a);
            itemSize = r.size();
        });

        return Serie(itemSize, r, this->dimension_);
    }

    template <typename F>
    inline Serie Serie::map(F &&cb) const
    {
        uint32_t itemSize = 0;
        Serie R;
        uint32_t id = 0;

        for (uint32_t i = 0; i < count_; ++i)
        {
            auto r = cb(value(i), i);

            if (itemSize == 0)
            {
                // Here we go! We got the itemSize for the new Serie :-)
                itemSize = r.size();
                R = Serie(itemSize, count_);
            }

            for (int j = 0; j < itemSize; ++j)
            {
                R.s_[id++] = r[j];
            }
        }
        return R;
    }

    template <typename F>
    Serie Serie::filter(F &&predicate) const
    {
        Array r;
        this->forEach( [&](const Array& a, uint32_t) {
            if (predicate(a) == true) {
                r.insert(std::end(r), std::begin(a), std::end(a));
            }
        });

        return Serie(this->itemSize(), r);
    }

    inline const Array &Serie::asArray() const
    {
        return s_;
    }

    inline Array &Serie::asArray()
    {
        return s_;
    }

    inline std::ostream &operator<<(std::ostream &o, Array a)
    {
        for (uint32_t i = 0; i < a.size(); ++i)
        {
            o << a[i] << " ";
        }
        return o;
    }

}
