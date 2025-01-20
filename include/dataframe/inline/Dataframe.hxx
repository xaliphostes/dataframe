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

#include <dataframe/functional/print.h>

namespace df {

// Wrapper pour le GenSerie concret
template <typename T>
SerieWrapper<T>::SerieWrapper(const GenSerie<T> &serie) : serie_(serie) {}

template <typename T> uint32_t SerieWrapper<T>::count() const {
    return serie_.count();
}

template <typename T> uint32_t SerieWrapper<T>::itemSize() const {
    return serie_.itemSize();
}

template <typename T> uint SerieWrapper<T>::dimension() const {
    return serie_.dimension();
}

template <typename T> bool SerieWrapper<T>::isValid() const {
    return serie_.isValid();
}

template <typename T>
void SerieWrapper<T>::print(uint32_t precision) const {
    df::print(serie_, precision);
}

template <typename T> std::string SerieWrapper<T>::type_name() const {
    return df::type_name<T>();
}

template <typename T>
std::unique_ptr<SerieBase> SerieWrapper<T>::clone() const {
    return std::make_unique<SerieWrapper<T>>(serie_);
}

template <typename T> const GenSerie<T> &SerieWrapper<T>::get() const {
    return serie_;
}

template <typename T> GenSerie<T> &SerieWrapper<T>::get() { return serie_; }

template <typename T>
void DataFrame::add(const std::string &name, const GenSerie<T> &serie) {
    if (series_.find(name) != series_.end()) {
        throw std::invalid_argument("Serie name already exists: " + name);
    }
    series_[name] = std::make_unique<SerieWrapper<T>>(serie);
}

// Récupérer une série avec son type
template <typename T>
const GenSerie<T> &DataFrame::get(const std::string &name) const {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::invalid_argument("Serie not found: " + name);
    }

    auto wrapper = dynamic_cast<const SerieWrapper<T> *>(it->second.get());
    if (!wrapper) {
        throw std::invalid_argument("Wrong serie type for: " + name);
    }

    return wrapper->get();
}

template <typename T> GenSerie<T> &DataFrame::get(const std::string &name) {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::invalid_argument("Serie not found: " + name);
    }

    auto wrapper = dynamic_cast<SerieWrapper<T> *>(it->second.get());
    if (!wrapper) {
        throw std::invalid_argument("Wrong serie type for: " + name);
    }

    return wrapper->get();
}

// Vérifier si une série existe
bool DataFrame::has(const std::string &name) const {
    return series_.find(name) != series_.end();
}

// Supprimer une série
void DataFrame::remove(const std::string &name) {
    if (!has(name)) {
        throw std::invalid_argument("Serie not found: " + name);
    }
    series_.erase(name);
}

// Obtenir les noms des séries
std::vector<std::string> DataFrame::names() const {
    std::vector<std::string> result;
    for (const auto &[name, _] : series_) {
        result.push_back(name);
    }
    return result;
}

// Obtenir le type d'une série
std::string DataFrame::get_type(const std::string &name) const {
    auto it = series_.find(name);
    if (it == series_.end()) {
        throw std::invalid_argument("Serie not found: " + name);
    }
    return it->second->type_name();
}

// Obtenir le nombre de séries
size_t DataFrame::size() const { return series_.size(); }

// Afficher le contenu
void DataFrame::print(uint32_t precision) const {
    std::cout << "DataFrame with " << size() << " series:" << std::endl;
    for (const auto &[name, serie] : series_) {
        std::cout << "\n"
                  << name << " (" << serie->type_name() << "):" << std::endl;
        serie->print(precision);
    }
}

} // namespace df