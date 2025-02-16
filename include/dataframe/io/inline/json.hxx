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
#include <dataframe/types.h>
#include <fstream>
#include "../nlohmann/json.hpp"
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
} // namespace detail

inline Dataframe read_json(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    Dataframe df;

    if (j.is_array()) {
        // Array of objects format
        std::map<std::string, std::vector<nlohmann::json>> columns;

        // First pass: collect all values
        for (const auto &row : j) {
            for (const auto &[key, value] : row.items()) {
                columns[key].push_back(value);
            }
        }

        // Second pass: determine types and create series
        for (const auto &[key, values] : columns) {
            bool is_number = true;
            bool is_int = true;
            bool is_string = false;

            for (const auto &value : values) {
                if (value.is_string()) {
                    is_number = false;
                    is_int = false;
                    is_string = true;
                    break;
                } else if (value.is_number_float()) {
                    is_int = false;
                } else if (!value.is_number()) {
                    is_number = false;
                    is_int = false;
                    break;
                }
            }

            if (is_int) {
                Serie<int64_t> serie;
                for (const auto &value : values) {
                    serie.add(value.get<int64_t>());
                }
                df.add(key, serie);
            } else if (is_number) {
                Serie<double> serie;
                for (const auto &value : values) {
                    serie.add(value.get<double>());
                }
                df.add(key, serie);
            } else if (is_string) {
                Serie<std::string> serie;
                for (const auto &value : values) {
                    serie.add(value.get<std::string>());
                }
                df.add(key, serie);
            }
        }
    }

    return df;
}

inline void write_json(const Dataframe &df, const std::string &filename,
                       bool pretty) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    auto names = df.names();
    if (names.empty()) {
        file << "[]";
        return;
    }

    nlohmann::json j = nlohmann::json::array();

    // Get size of first serie to determine number of rows
    size_t num_rows = df.get<SerieBase>(names[0]).size();

    // Create array of objects format
    for (size_t row = 0; row < num_rows; ++row) {
        nlohmann::json row_obj;

        for (const auto &name : names) {
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

    // Write to file with appropriate formatting
    if (pretty) {
        file << j.dump(4);
    } else {
        file << j.dump();
    }
}

} // namespace io
} // namespace df