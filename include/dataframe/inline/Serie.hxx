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

#include "../utils.h"
#include <cxxabi.h>
#include <memory>
#include <ostream>
#include <regex>

namespace df {

// ------------------------------------------

inline std::string cleanup_type_name(const std::string &name) {
    std::string result = name;

    // List of the substitutions to clean the name
    static const std::vector<std::pair<std::string, std::string>>
        substitutions = {
            {"std::__1::", "std::"},     // Enlève le __1 de l'espace de nom std
            {"std::vector<", "vector<"}, // Simplifie std::vector
            {"std::array<", "array<"},   // Simplifie std::array
            {", std::allocator<[^>]+>>", ">"}, // Enlève l'allocator
            {", std::allocator<[^>]+>", ">"}, // Enlève l'allocator (cas simple)
            {"ul", ""},
            {"std::basic_string<char>", "string"}, // Simplifie basic_string
            {"std::basic_string<char, std::char_traits<char>>",
             "string"}, // Autre forme de string
        };

    // Apply every substitution
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

// ------------------------------------------------

template <typename T> Serie<T>::Serie(const ArrayType &values) {
    data_.reserve(values.size());
    for (const auto &v : values) {
        data_.push_back(v);
    }
}

template <typename T> Serie<T>::Serie(const std::initializer_list<T> &values) {
    data_.reserve(values.size());
    for (auto &v : values) {
        data_.push_back(v);
    }
}

template <typename T> const Serie<T>::ArrayType &Serie<T>::data() const {
    return data_;
}

template <typename T> const Serie<T>::ArrayType &Serie<T>::asArray() const {
    return data_;
}

template <typename T> bool Serie<T>::empty() const { return data_.empty(); }

template <typename T> std::string Serie<T>::type() const {
    return type_name<T>();
}

template <typename T> T &Serie<T>::operator[](size_t index) {
    if (index >= data_.size()) {
        throw std::out_of_range(format("Index ", index,
                                       " is out of bounds (max is ",
                                       data_.size(), ") in Serie::operator[]"));
    }
    return data_[index];
}

template <typename T> const T &Serie<T>::operator[](size_t index) const {
    if (index >= data_.size()) {
        throw std::out_of_range(format("Index ", index,
                                       " is out of bounds (max is ",
                                       data_.size(), ") in Serie::operator[]"));
    }
    return data_[index];
}

template <typename T> size_t Serie<T>::size() const { return data_.size(); }

template <typename T>
template <typename F>
inline void Serie<T>::forEach(F &&callback) const {
    for (size_t i = 0; i < data_.size(); ++i) {
        callback(data_[i], i);
    }
}

template <typename T>
template <typename F>
inline auto Serie<T>::map(F &&callback) const {
    using ResultType = decltype(callback(data_[0], 0));
    std::vector<ResultType> result(data_.size());

    for (size_t i = 0; i < data_.size(); ++i) {
        result[i] = callback(data_[i], i);
    }
    return Serie<ResultType>(result);
}

template <typename T>
template <typename F, typename AccT>
auto Serie<T>::reduce(F &&callback, AccT initial) {
    AccT result = initial;

    for (size_t i = 0; i < data_.size(); ++i) {
        result = callback(result, data_[i], i);
    }
    return result;
}

} // namespace df

// -----------------------------------------------

template <typename T>
std::ostream &operator<<(std::ostream &o, const df::Serie<T> &s) {
    o << "Serie<" << s.type_name() << ">" << std::endl;
    o << "  size    : " << s.size() << std::endl;
    if (s.size() > 0) {
        o << "  values   : [";
        const auto &v = s.data();
        for (size_t i = 0; i < v.size() - 1; ++i) {
            o << v[i] << ", ";
        }
        o << v[v.size() - 1] << "]";
    } else {
        o << "  values   : []";
    }
    return o;
}