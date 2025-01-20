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

#include <cxxabi.h>
#include <memory>
#include <regex>

namespace df {

inline std::string cleanup_type_name(const std::string &name) {
    std::string result = name;

    // Liste des substitutions pour nettoyer le nom
    static const std::vector<std::pair<std::string, std::string>>
        substitutions = {
            {"std::__1::", "std::"}, // Enlève le __1 de l'espace de nom std
            {"std::vector<", "vector<"},       // Simplifie std::vector
            {", std::allocator<[^>]+>>", ">"}, // Enlève l'allocator
            {", std::allocator<[^>]+>", ">"}, // Enlève l'allocator (cas simple)
            {"std::basic_string<char>", "string"}, // Simplifie basic_string
            {"std::basic_string<char, std::char_traits<char>>",
             "string"}, // Autre forme de string
        };

    // Applique chaque substitution
    for (const auto &[pattern, replacement] : substitutions) {
        result = std::regex_replace(result, std::regex(pattern), replacement);
    }

    return result;
}

template <typename T> std::string type_name() {
    const char *mangled = typeid(T).name();
    int status = 0;
    std::unique_ptr<char, void (*)(void *)> demangled(
        abi::__cxa_demangle(mangled, nullptr, nullptr, &status), std::free);
    return cleanup_type_name(status == 0 ? demangled.get() : mangled);
}

template <typename T> std::string GenSerie<T>::type() const {
    return type_name<T>();
}

template <typename T>
GenSerie<T>::GenSerie(int itemSize, uint32_t count, uint dimension)
    : itemSize_(itemSize), count_(count), dimension_(dimension) {
    s_ = Array(count * itemSize);
}

template <typename T>
GenSerie<T>::GenSerie(int itemSize, const Array &values, uint dimension)
    : itemSize_(itemSize), dimension_(dimension) {
    count_ = values.size() / itemSize;
    s_ = values;
}

template <typename T>
GenSerie<T>::GenSerie(int itemSize, const std::initializer_list<T> &values,
                      uint dimension)
    : itemSize_(itemSize), dimension_(dimension) {
    count_ = values.size() / itemSize;
    s_ = Array(values);
}

template <typename T>
GenSerie<T>::GenSerie(const GenSerie &s)
    : itemSize_(s.itemSize_), count_(s.count_), dimension_(s.dimension_) {
    s_ = Array(s.s_.cbegin(), s.s_.cend());
}

template <typename T>
GenSerie<T> GenSerie<T>::create(int itemSize, const Array &data,
                                uint dimension) {
    return GenSerie(itemSize, data, dimension);
}

template <typename T> bool GenSerie<T>::isValid() const {
    return itemSize_ > 0;
}

template <typename T> bool GenSerie<T>::isEmpty() const { return count_ == 0; }

template <typename T> void GenSerie<T>::reCount(uint32_t c) {
    count_ = c;
    s_ = Array(c * itemSize_);
}

template <typename T>
GenSerie<T> &GenSerie<T>::operator=(const GenSerie<T> &s) {
    count_ = s.count_;
    itemSize_ = s.itemSize_;
    s_ = Array(s.s_.cbegin(), s.s_.cend());
    return *this;
}

template <typename T> GenSerie<T> GenSerie<T>::clone() const {
    return GenSerie<T>(itemSize_, s_);
}

template <typename T> uint32_t GenSerie<T>::size() const {
    return count_ * itemSize_;
}

template <typename T> uint32_t GenSerie<T>::count() const { return count_; }

template <typename T> uint32_t GenSerie<T>::itemSize() const {
    return itemSize_;
}

template <typename T> uint GenSerie<T>::dimension() const { return dimension_; }

template <typename T> GenSerie<T>::Array GenSerie<T>::array(uint32_t i) const {
    auto start = i * itemSize_;
    Array r(itemSize_);
    for (uint32_t j = 0; j < itemSize_; ++j) {
        r[j] = s_[start + j];
    }
    return r;
}

template <typename T> T GenSerie<T>::value(uint32_t i) const {
    if (i > size()) {
        throw std::invalid_argument("index out of bounds (" +
                                    std::to_string(i) +
                                    ">=" + std::to_string(size()));
    }
    return s_[i];
}

template <typename T> void GenSerie<T>::setArray(uint32_t i, const Array &v) {
    if (i >= count_) {
        throw std::invalid_argument("index out of range (" + std::to_string(i) +
                                    ">=" + std::to_string(count_) + ")");
    }

    auto size = itemSize_;
    if (v.size() != size) {
        throw std::invalid_argument(
            "provided item size (" + std::to_string(v.size()) +
            ") is different from itemSize (" + std::to_string(itemSize_) + ")");
    }
    for (int j = 0; j < size; ++j) {
        s_[i * size + j] = v[j];
    }
}

template <typename T> void GenSerie<T>::setValue(uint32_t i, T v) {
    if (i >= s_.size()) {
        throw std::invalid_argument("index out of range (" + std::to_string(i) +
                                    ">=" + std::to_string(s_.size()) + ")");
    }
    s_[i] = v;
}

template <typename T> const GenSerie<T>::Array &GenSerie<T>::asArray() const {
    return s_;
}

template <typename T> GenSerie<T>::Array &GenSerie<T>::asArray() { return s_; }

template <typename T>
template <typename U>
auto GenSerie<T>::get(uint32_t i) const
    -> std::conditional_t<details::is_array_v<U>, Array, T> {
    if constexpr (details::is_array_v<U>) {
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

template <typename T>
template <typename U>
void GenSerie<T>::set(uint32_t i, const U &value) {
    if constexpr (details::is_array_v<U>) {
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

template <typename T>
template <typename F>
void GenSerie<T>::forEach(F &&cb) const {
    for (uint32_t i = 0; i < count_; ++i) {
        cb(value(i), i);
    }
}

template <typename T>
template <typename F>
auto GenSerie<T>::map(F &&cb) const {
    // Return type deduction based on callback
    using ReturnType =
        std::decay_t<decltype(cb(std::declval<Array>(), uint32_t{}))>;
    GenSerie<ReturnType> result(itemSize_, count_);

    for (uint32_t i = 0; i < count_; ++i) {
        result.setValue(i, cb(value(i), i));
    }
    return result;
}

} // namespace df

template <typename T>
std::ostream &operator<<(std::ostream &o, const df::GenSerie<T> &s) {
    o << "GenSerie<" << typeid(T).name() << ">:" << std::endl;
    o << "  itemSize : " << s.itemSize() << std::endl;
    o << "  count    : " << s.count() << std::endl;
    o << "  dimension: " << s.dimension() << std::endl;
    if (s.count() > 0) {
        o << "  values   : [";
        auto v = s.asArray();
        for (uint32_t i = 0; i < v.size() - 1; ++i) {
            o << v[i] << ", ";
        }
        o << v[v.size() - 1] << "]";
    } else {
        o << "  values   : []";
    }
    return o;
}