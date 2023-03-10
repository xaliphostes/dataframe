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
template <typename F>
inline void Serie::forEach(F &&cb) const
{
    for (uint32_t i = 0; i < count_; ++i)
    {
        cb(itemAt(i), i);
    }
}

template <typename F>
Array Serie::reduce(F &&reduceFn, const Array &acc)
{
    return std::accumulate(s_.begin(), s_.end(), acc, [reduceFn, acc] (Array previousResult, const Array& item) {
        return reduceFn(previousResult, item) ;
    }) ;

    // auto cumul = acc;
    // forEach([reduceFn, cumul](Array v, uint32_t i)
    //         { cumul = reduceFn(cumul, v); });
    // return cumul;
}

template <typename F>
inline Serie Serie::map(F &&cb) const
{
    auto tmp = cb(itemAt(0), 0);
    auto itemSize = tmp.size();
    auto R = Serie(itemSize, count_);
    uint32_t id = 0;
    for (uint32_t i = 0; i < count_; ++i)
    {
        auto r = cb(itemAt(i), i);
        for (int j = 0; j < itemSize; ++j)
        {
            R.s_[id++] = r[j];
        }
    }
    return R;
}

inline const Array &Serie::array() const
{
    return s_;
}

inline Array &Serie::array()
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
