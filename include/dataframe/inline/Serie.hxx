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

#include "../functional/filter.h"
#include "../functional/forEach.h"
#include "../functional/map.h"
#include "../functional/pipe.h"
#include "../functional/reduce.h"
#include "../utils/inferring.h"

namespace df {

template <typename F> void Serie::forEach(F &&cb) const {
    df::forEach(cb, *this);
}

template <typename F> auto Serie::map(F &&cb) const {
    return df::map(cb, *this);
}

template <typename F> auto Serie::reduce(F &&cb, double init) {
    return df::reduce(*this, cb, init);
}

template <typename F> auto Serie::reduce(F &&cb, const Array &init) {
    return df::reduce(*this, cb, init);
}

template <typename F> auto Serie::filter(F &&predicate) const {
    return df::filter(predicate, *this);
}

template <typename F> auto Serie::pipe(F &&op) const {
    return df::pipe(op, *this);
}

template <typename F, typename... Fs>
auto Serie::pipe(F &&op, Fs &&...ops) const {
    return df::pipe(*this, op, ops...);
}

inline const Array &Serie::asArray() const { return s_; }

inline Array &Serie::asArray() { return s_; }

inline std::ostream &operator<<(std::ostream &o, const Serie &s) {
    std::cerr << "Serie:" << std::endl;
    std::cerr << "  itemSize : " << s.itemSize() << std::endl;
    std::cerr << "  count    : " << s.count() << std::endl;
    std::cerr << "  dimension: " << s.dimension() << std::endl;
    if (s.count() > 0) {
        std::cerr << "  values   : [";
        Array v = s.asArray();
        for (uint32_t i = 0; i < v.size() - 1; ++i) {
            std::cerr << v[i] << ", ";
        }
        std::cerr << v[v.size() - 1] << "]";
    } else {
        std::cerr << "  values   : []";
    }
    return o;
}

/**
 * @brief Unified get method that handles both scalar and Array cases
 * @tparam T Return type (deduced automatically)
 * @param i Index
 * @return Either a double (scalar) or an Array based on itemSize
 *
 * @example
 * ```cpp
 * Serie s1(1, {1, 2, 3});
 * double val = s1.get<double>(0);    // Retourne 1.0
 *
 * Serie s2(3, {1,2,3, 4,5,6});
 * Array vec = s2.get<Array>(0);      // Retourne {1,2,3}
 *
 * // Ou simplement avec déduction automatique:
 * auto val = s1.get(0);  // double
 * auto vec = s2.get(0);  // Array
 * ```
 */
template <typename T>
inline auto Serie::get(uint32_t i) const
    -> std::conditional_t<details::is_array_v<T>, Array, double> {
    if constexpr (details::is_array_v<T>) {
        if (itemSize_ == 1) {
            return Array{s_[i]};
        }
        Array r(itemSize_);
        for (uint32_t j = 0; j < itemSize_; ++j) {
            r[j] = s_[i * itemSize_ + j];
        }
        return r;
    } else {
        if (i >= s_.size()) {
            throw std::invalid_argument("index out of bounds (" +
                                        std::to_string(i) +
                                        ">=" + std::to_string(s_.size()) + ")");
        }
        return s_[i];
    }
}

/**
 * @brief Unified set method that handles both scalar and Array cases
 * @tparam T Input type (deduced automatically)
 * @param i Index
 * @param value Value to set (either double or Array)
 *
 * @example
 * ```cpp
 * Serie s1(1, {1, 2, 3});
 * s1.set(0, 42.0);           // Set scalar value
 *
 * Serie s2(3, {1,2,3, 4,5,6});
 * s2.set(0, Array{7,8,9});   // Set vector value
 * ```
 */
template <typename T> inline void Serie::set(uint32_t i, const T &value) {
    if constexpr (details::is_array_v<T>) {
        if (i >= count_) {
            throw std::invalid_argument("index out of range (" +
                                        std::to_string(i) +
                                        ">=" + std::to_string(count_) + ")");
        }
        if (value.size() != itemSize_) {
            throw std::invalid_argument("provided item size (" +
                                        std::to_string(value.size()) +
                                        ") is different from itemSize (" +
                                        std::to_string(itemSize_) + ")");
        }
        for (uint32_t j = 0; j < itemSize_; ++j) {
            s_[i * itemSize_ + j] = value[j];
        }
    } else {
        if (i >= s_.size()) {
            throw std::invalid_argument("index out of bounds (" +
                                        std::to_string(i) +
                                        ">=" + std::to_string(s_.size()) + ")");
        }
        s_[i] = value;
    }
}

} // namespace df
