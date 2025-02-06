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

// Dataframe.hxx
#include "../utils.h"

namespace df {

template <typename T>
void Dataframe::add(const std::string &name, const Serie<T> &serie) {
    if (has(name)) {
        throw std::runtime_error(
            format("Serie with name '", name, "' already exists in Dataframe"));
    }
    series_.emplace(name, SerieInfo(serie));
}

template <typename T>
void Dataframe::add(const std::string &name, const ArrayType<T> &array) {
    add(name, Serie<T>(array));
}

inline void Dataframe::remove(const std::string &name) {
    if (!has(name)) {
        throw std::runtime_error(
            format("Serie '", name, "' does not exist in Dataframe"));
    }
    series_.erase(name);
}

template <typename T>
const Serie<T> &Dataframe::get(const std::string &name) const {
    if (!has(name)) {
        throw std::runtime_error(
            format("Serie '", name, "' does not exist in Dataframe"));
    }

    const auto &info = series_.at(name);
    auto typed_ptr = std::dynamic_pointer_cast<Serie<T>>(info.data);

    if (!typed_ptr) {
        throw std::runtime_error(format(
            "Type mismatch for Serie '", name, "': expected type '",
            typeid(Serie<T>).name(), "' but got '", info.type.name(), "'"));
    }

    return *typed_ptr;
}

inline std::type_index Dataframe::type(const std::string &name) const {
    if (!has(name)) {
        throw std::runtime_error(
            format("Serie '", name, "' does not exist in Dataframe"));
    }
    return series_.at(name).type;
}

inline String Dataframe::type_name(const std::string &name) const {
    if (!has(name)) {
        throw std::runtime_error(
            format("Serie '", name, "' does not exist in Dataframe"));
    }
    return series_.at(name).data.operator*().type();
}

inline bool Dataframe::has(const std::string &name) const {
    return series_.find(name) != series_.end();
}

inline size_t Dataframe::size() const { return series_.size(); }

inline std::vector<std::string> Dataframe::names() const {
    std::vector<std::string> result;
    result.reserve(series_.size());
    for (const auto &[name, _] : series_) {
        result.push_back(name);
    }
    return result;
}

inline void Dataframe::clear() { series_.clear(); }

} // namespace df