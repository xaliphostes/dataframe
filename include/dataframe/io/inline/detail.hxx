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

namespace df {
namespace io {

namespace detail {

inline std::string trim(const std::string &str) {
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    const auto end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::vector<std::string> split_line(const std::string &line,
                                           char delimiter, char quote) {
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;

    for (char c : line) {
        if (c == quote) {
            in_quotes = !in_quotes;
        } else if (c == delimiter && !in_quotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

template <typename T>
inline Serie<T> parse_column(const std::vector<std::string> &values) {
    Serie<T> serie;
    serie.reserve(values.size());

    for (const auto &value : values) {
        if constexpr (std::is_same_v<T, std::string>) {
            serie.add(value);
        } else if constexpr (std::is_arithmetic_v<T>) {
            if (value.empty()) {
                serie.add(T{});
            } else {
                std::istringstream iss(value);
                T parsed_value;
                iss >> parsed_value;
                serie.add(parsed_value);
            }
        }
    }
    return serie;
}

inline std::type_index
infer_column_type(const std::vector<std::string> &values, bool all_double) {
    bool could_be_int = true;
    bool could_be_double = true;

    if (all_double) {
        return typeid(double);
    }

    for (const auto &value : values) {
        if (value.empty())
            continue;

        try {
            std::size_t pos;
            std::stoll(value, &pos);
            if (pos != value.length())
                could_be_int = false;
        } catch (...) {
            could_be_int = false;
        }

        if (!could_be_int) {
            try {
                std::stod(value);
            } catch (...) {
                could_be_double = false;
                break;
            }
        }
    }

    if (could_be_int)
        return typeid(int64_t);
    if (could_be_double)
        return typeid(double);
    return typeid(std::string);
}

template <typename T> inline std::string format_value(const T &value) {
    if constexpr (std::is_same_v<T, std::string>) {
        // Check if we need to quote the string
        if (value.find(',') != std::string::npos ||
            value.find('"') != std::string::npos ||
            value.find('\n') != std::string::npos) {
            std::string escaped = value;
            // Escape any quotes in the string
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.insert(pos, 1, '"');
                pos += 2;
            }
            return '"' + escaped + '"';
        }
        return value;
    } else {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

} // namespace detail
} // namespace io
} // namespace df