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

#include "../detail.h"
#include "../nlohmann/json.hpp"
#include <dataframe/types.h>
#include <fstream>
#include <sstream>
#include <string>

namespace df {
namespace io {

namespace detail {

template <typename T> nlohmann::json serie_to_json(const Serie<T> &serie) {
    nlohmann::json j = nlohmann::json::array();
    serie.forEach([&j](const T &value) { j.push_back(value); });
    return j;
}

template <typename T> Serie<T> json_to_serie(const nlohmann::json &json) {
    if (!json.is_array()) {
        throw std::runtime_error(
            "JSON value must be an array to convert to Serie");
    }

    Serie<T> serie;
    serie.reserve(json.size());
    for (const auto &value : json) {
        serie.add(value.get<T>());
    }
    return serie;
}

inline bool is_numeric_array(const std::vector<nlohmann::json> &values,
                             bool &is_integer) {
    is_integer = true;
    for (const auto &value : values) {
        if (!value.is_number())
            return false;
        if (value.is_number_float())
            is_integer = false;
    }
    return true;
}
} // namespace detail

inline Dataframe read_json(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    Dataframe df;
    if (!j.is_array()) {
        throw std::runtime_error("JSON root must be an array");
    }

    // Early return for empty array
    if (j.empty())
        return df;

    // Collect all unique keys and their values
    std::map<std::string, std::vector<nlohmann::json>> columns;
    for (const auto &row : j) {
        if (!row.is_object()) {
            throw std::runtime_error("Each array element must be an object");
        }
        for (const auto &[key, value] : row.items()) {
            columns[key].push_back(value);
        }
    }

    // Create series based on value types
    for (const auto &[key, values] : columns) {
        // Check if all values are strings
        if (std::all_of(values.begin(), values.end(),
                        [](const auto &v) { return v.is_string(); })) {
            Serie<std::string> serie;
            serie.reserve(values.size());
            for (const auto &value : values) {
                serie.add(value.get<std::string>());
            }
            df.add(key, serie);
            continue;
        }

        // Check if all values are numeric
        bool is_integer;
        if (detail::is_numeric_array(values, is_integer)) {
            if (is_integer) {
                Serie<int64_t> serie;
                serie.reserve(values.size());
                for (const auto &value : values) {
                    serie.add(value.get<int64_t>());
                }
                df.add(key, serie);
            } else {
                Serie<double> serie;
                serie.reserve(values.size());
                for (const auto &value : values) {
                    serie.add(value.get<double>());
                }
                df.add(key, serie);
            }
            continue;
        }

        // Default to string for mixed or unsupported types
        Serie<std::string> serie;
        serie.reserve(values.size());
        for (const auto &value : values) {
            serie.add(value.dump());
        }
        df.add(key, serie);
    }

    return df;
}

inline void write_json(const Dataframe &df, const std::string &filename,
                       bool pretty) {

    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    write_json(df, file, pretty);
}

void write_json(const Dataframe &df, std::ostream &file, bool pretty) {
    if (df.size() == 0) {
        file << "[]";
        return;
    }

    nlohmann::json j = nlohmann::json::array();
    size_t num_rows = df.begin()->second.data->size();

    // Create array of objects format
    for (size_t row = 0; row < num_rows; ++row) {
        nlohmann::json row_obj;

        for (const auto &serie_pair : df) {
            const auto &name = serie_pair.first;
            const auto &serie_type = df.type(name);

            if (serie_type == typeid(Serie<int64_t>)) {
                row_obj[name] = df.get<int64_t>(name)[row];
            } else if (serie_type == typeid(Serie<double>)) {
                row_obj[name] = df.get<double>(name)[row];
            } else if (serie_type == typeid(Serie<std::string>)) {
                row_obj[name] = df.get<std::string>(name)[row];
            }
        }
        j.push_back(row_obj);
    }

    file << (pretty ? j.dump(4) : j.dump());
}

} // namespace io
} // namespace df