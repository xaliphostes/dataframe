/*
 * Copyright (c) 2024-now fmaerten@gmail.com
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
 */

#include <stdexcept>
#include <string>
#include <vector>

namespace df {

inline LabelEncoder &LabelEncoder::fit(const df::Serie<std::string> &serie) {
    // Clear existing mappings
    string_to_id_.clear();
    id_to_string_.clear();

    // Assign a unique ID to each unique string value
    size_t next_id = 0;

    for (size_t i = 0; i < serie.size(); ++i) {
        const std::string &value = serie[i];

        // If this string hasn't been seen before, add it to the mappings
        if (string_to_id_.find(value) == string_to_id_.end()) {
            string_to_id_[value] = next_id;
            id_to_string_[next_id] = value;
            next_id++;
        }
    }

    fitted_ = true;
    return *this;
}

inline df::Serie<double>
LabelEncoder::transform(const df::Serie<std::string> &serie) const {
    if (!fitted_) {
        throw std::runtime_error(
            "LabelEncoder must be fitted before transform");
    }

    std::vector<double> encoded;
    encoded.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        const std::string &value = serie[i];
        auto it = string_to_id_.find(value);

        if (it == string_to_id_.end()) {
            throw std::runtime_error("Unknown category encountered: " + value);
        }

        encoded.push_back(static_cast<double>(it->second));
    }

    return df::Serie<double>(encoded);
}

inline df::Serie<double>
LabelEncoder::fit_transform(const df::Serie<std::string> &serie) {
    fit(serie);
    return transform(serie);
}

inline df::Serie<std::string>
LabelEncoder::inverse_transform(const df::Serie<double> &serie) const {
    if (!fitted_) {
        throw std::runtime_error(
            "LabelEncoder must be fitted before inverse_transform");
    }

    std::vector<std::string> decoded;
    decoded.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        // Convert to size_t for lookup
        size_t id = static_cast<size_t>(serie[i]);
        auto it = id_to_string_.find(id);

        if (it == id_to_string_.end()) {
            throw std::runtime_error("Unknown ID encountered: " +
                                     std::to_string(id));
        }

        decoded.push_back(it->second);
    }

    return df::Serie<std::string>(decoded);
}

inline bool LabelEncoder::is_fitted() const { return fitted_; }

inline size_t LabelEncoder::num_categories() const {
    return string_to_id_.size();
}

inline std::vector<std::string> LabelEncoder::categories() const {
    std::vector<std::string> result;
    result.reserve(string_to_id_.size());

    for (const auto &pair : string_to_id_) {
        result.push_back(pair.first);
    }

    return result;
}

} // namespace df