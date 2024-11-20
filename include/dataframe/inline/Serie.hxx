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
#include "../algos/forEach.h"
#include "../algos/reduce.h"
#include "../algos/map.h"
#include "../algos/filter.h"
#include "../algos/pipe.h"

namespace df
{

    /* OLD IMPL
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
    */

    template <typename F>
    void Serie::forEach(F &&cb) const  {
        df::forEach(*this, cb);
    }

    template <typename F>
    Serie Serie::map(F &&cb) const  {
        return df::map(*this, cb);
    }

    template <typename F>
    auto Serie::reduce(F &&cb, double init) {
        return df::reduce(*this, cb, init);
    }

    template <typename F>
    auto Serie::reduce(F &&cb, const Array &init) {
        return df::reduce(*this, cb, init);
    }

    template <typename F>
    Serie Serie::filter(F &&predicate) const  {
        return df::filter(*this, predicate);
    }

    template <typename F>
    Serie Serie::pipe(F &&op) const
    {
        return df::pipe(*this, op);
    }

    template <typename F, typename... Fs>
    Serie Serie::pipe(F &&op, Fs &&...ops) const
    {
        return df::pipe(*this, op, ops...);
    }





    template <typename F>
    inline void Serie::forEachScalar(F &&cb) const
    {
        if (itemSize_ != 1)
        {
            throw std::invalid_argument("(forEachScalar: itemSize is not 1 (got " + std::to_string(itemSize_) + ")");
        }

        for (uint32_t i = 0; i < count_; ++i)
        {
            cb(scalar(i), i);
        }
    }

    template <typename F>
    double Serie::reduceScalar(F &&reduceFn, double acc) const
    {
        if (itemSize_ != 1)
        {
            throw std::invalid_argument("reduceScalar: itemSize is not 1 (got " + std::to_string(itemSize_) + ")");
        }

        double r = acc;
        this->forEachScalar([&](double a, uint32_t index)
                            { r = reduceFn(r, a, index); });

        return r; // Serie(1, Array{r}, this->dimension_);
    }

    // template <typename F>
    // inline Serie Serie::mapScalar(F &&cb) const
    // {
    //     if (itemSize_ != 1)
    //     {
    //         throw std::invalid_argument("mapScalar: itemSize is not 1 (got " + std::to_string(itemSize_) + ")");
    //     }

    //     Serie R(1, count_);
    //     uint32_t id = 0;

    //     for (uint32_t i = 0; i < count_; ++i)
    //     {
    //         R.s_[id++] = cb(scalar(i), i);
    //     }
    //     return R;
    // }

    inline const Array &Serie::asArray() const
    {
        return s_;
    }

    inline Array &Serie::asArray()
    {
        return s_;
    }

    inline std::ostream &operator<<(std::ostream &o, const Serie &s)
    {
        std::cerr << "Serie:" << std::endl;
        std::cerr << "  itemSize : " << s.itemSize() << std::endl;
        std::cerr << "  count    : " << s.count() << std::endl;
        std::cerr << "  dimension: " << s.dimension() << std::endl;
        std::cerr << "  values   : [";
        Array v = s.asArray();
        for (uint32_t i=0; i<v.size()-1; ++i)
        {
            std::cerr << v[i] << ", ";
        }
        std::cerr << v[v.size()-1] << "]";
        return o;
    }

    template <typename T>
    inline auto Serie::get(uint32_t i) const -> std::conditional_t<detail::is_array_v<T>, Array, double>
    {
        if constexpr (detail::is_array_v<T>)
        {
            if (itemSize_ == 1)
            {
                return Array{s_[i]};
            }
            Array r(itemSize_);
            for (uint32_t j = 0; j < itemSize_; ++j)
            {
                r[j] = s_[i * itemSize_ + j];
            }
            return r;
        }
        else
        {
            if (i >= s_.size())
            {
                throw std::invalid_argument("index out of bounds (" +
                                            std::to_string(i) + ">=" + std::to_string(s_.size()) + ")");
            }
            return s_[i];
        }
    }

    template <typename T>
    inline void Serie::set(uint32_t i, const T &value)
    {
        if constexpr (detail::is_array_v<T>)
        {
            if (i >= count_)
            {
                throw std::invalid_argument("index out of range (" +
                                            std::to_string(i) + ">=" + std::to_string(count_) + ")");
            }
            if (value.size() != itemSize_)
            {
                throw std::invalid_argument("provided item size (" +
                                            std::to_string(value.size()) + ") is different from itemSize (" +
                                            std::to_string(itemSize_) + ")");
            }
            for (uint32_t j = 0; j < itemSize_; ++j)
            {
                s_[i * itemSize_ + j] = value[j];
            }
        }
        else
        {
            if (i >= s_.size())
            {
                throw std::invalid_argument("index out of bounds (" +
                                            std::to_string(i) + ">=" + std::to_string(s_.size()) + ")");
            }
            s_[i] = value;
        }
    }

}
