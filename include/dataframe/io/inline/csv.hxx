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
#include <sstream>
#include <string>

namespace df {
namespace io {

inline Dataframe read_csv(const std::string &filename,
                          const CSVOptions &options) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    Dataframe df;
    std::string line;
    std::vector<std::string> headers;
    std::map<std::string, std::vector<std::string>> columns;

    // Skip initial rows
    for (size_t i = 0; i < options.skip_rows; ++i) {
        if (!std::getline(file, line))
            return df;
    }

    // Read header
    if (options.has_header) {
        if (!std::getline(file, line))
            return df;
        headers =
            detail::split_line(line, options.delimiter, options.quote_char);
        if (options.trim_whitespace) {
            for (auto &header : headers) {
                header = detail::trim(header);
            }
        }
    }

    // Read data rows
    while (std::getline(file, line)) {
        if (options.skip_empty_lines && detail::trim(line).empty())
            continue;

        auto fields =
            detail::split_line(line, options.delimiter, options.quote_char);
        if (options.trim_whitespace) {
            for (auto &field : fields) {
                field = detail::trim(field);
            }
        }

        // Initialize headers if not present
        if (headers.empty()) {
            headers.reserve(fields.size());
            for (size_t i = 0; i < fields.size(); ++i) {
                headers.push_back("Column" + std::to_string(i));
            }
        }

        // Store fields in columns
        for (size_t i = 0; i < std::min(headers.size(), fields.size()); ++i) {
            columns[headers[i]].push_back(
                fields[i] == options.null_value ? "" : fields[i]);
        }
    }

    // Convert columns to Series
    for (const auto &header : headers) {
        const auto &values = columns[header];
        auto type = detail::infer_column_type(values, options.all_double);

        if (type == typeid(int64_t)) {
            df.add(header, detail::parse_column<int64_t>(values));
        } else if (type == typeid(double)) {
            df.add(header, detail::parse_column<double>(values));
        } else {
            df.add(header, detail::parse_column<std::string>(values));
        }
    }

    return df;
}

inline void write_csv(const Dataframe &df, std::ostream &os,
                      const CSVOptions &options) {
    if (df.size() == 0)
        return;

    // Write header
    if (options.has_header) {
        bool first = true;
        for (const auto &serie_pair : df) {
            if (!first)
                os << options.delimiter;
            os << detail::format_value(serie_pair.first);
            first = false;
        }
        os << '\n';
    }

    // Get number of rows from first serie
    size_t num_rows = df.begin()->second.data->size();

    // Write data rows
    for (size_t row = 0; row < num_rows; ++row) {
        bool first = true;
        for (const auto &serie_pair : df) {
            if (!first)
                os << options.delimiter;

            const auto &name = serie_pair.first;
            const auto &serie_type = df.type(name);

            if (serie_type == typeid(Serie<int64_t>)) {
                os << detail::format_value(df.get<int64_t>(name)[row]);
            } else if (serie_type == typeid(Serie<double>)) {
                os << detail::format_value(df.get<double>(name)[row]);
            } else if (serie_type == typeid(Serie<std::string>)) {
                std::string value = df.get<std::string>(name)[row];
                os << (value.empty() ? options.null_value
                                     : detail::format_value(value));
            }
            first = false;
        }
        os << '\n';
    }
}

inline void write_csv(const Dataframe &df, const std::string &filename,
                      const CSVOptions &options) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    write_csv(df, file, options);
}

} // namespace io
} // namespace df
