#include "../common.h"

namespace df {

template <typename T>
void DataFrame::add(const std::string &name, const Serie<T> &serie) {
    if (has(name)) {
        throw std::runtime_error(
            details::format("Serie with name '", name, "' already exists in DataFrame"));
    }

    // Store the serie as a shared_ptr to void, but internally it's typed
    series_[name] = std::make_shared<Serie<T>>(serie);
}

template <typename T>
void DataFrame::add(const std::string &name, const ArrayType<T> &array) {
    if (has(name)) {
        throw std::runtime_error(
            details::format("Serie with name '", name, "' already exists in DataFrame"));
    }

    // Store the serie as a shared_ptr to void, but internally it's typed
    series_[name] = std::make_shared<Serie<T>>(Serie<T>(array));
}

inline void DataFrame::remove(const std::string &name) {
    if (!has(name)) {
        throw std::runtime_error(
            details::format("Serie '", name, "' does not exist in DataFrame"));
    }
    series_.erase(name);
}

template <typename T>
const Serie<T> &DataFrame::get(const std::string &name) const {
    if (!has(name)) {
        throw std::runtime_error(
            details::format("Serie '", name, "' does not exist in DataFrame"));
    }

    auto serie_ptr = std::static_pointer_cast<Serie<T>>(series_.at(name));
    if (!serie_ptr) {
        throw std::runtime_error(details::format("Type mismatch: Serie '", name,
                                        "' is not of type ", getTypeName<T>()));
    }

    return *serie_ptr;
}

inline bool DataFrame::has(const std::string &name) const {
    return series_.find(name) != series_.end();
}

inline size_t DataFrame::size() const { return series_.size(); }

inline std::vector<std::string> DataFrame::names() const {
    std::vector<std::string> result;
    result.reserve(series_.size());
    for (const auto &[name, _] : series_) {
        result.push_back(name);
    }
    return result;
}

inline void DataFrame::clear() { series_.clear(); }

template <typename T> std::string DataFrame::getTypeName() const {
    return type_name<T>();
}

} // namespace df