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
#include "../utils/utils.h"
#include <dataframe/math/bounds.h>
#include <dataframe/stats/avg.h>
#include <dataframe/stats/quantile.h>

namespace df {

template <typename T>
void Dataframe::add(const std::string &name, const Serie<T> &serie) {
    if (has<T>(name)) {
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

template <typename T> bool Dataframe::has(const std::string &name) const {
    try {
        // First check if the serie exists
        if (!series_.contains(name)) {
            return false;
        }

        // Get the stored type info
        const auto &serieInfo = series_.at(name);

        // Compare stored type with requested type
        return serieInfo.type == std::type_index(typeid(Serie<T>));
    } catch (const std::exception &) {
        return false;
    }
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

// ---------------------------------------------------------
// ------------------------ PRINT --------------------------
// ---------------------------------------------------------

namespace detail {
// Helper to check if a type is numeric
template <typename T>
struct is_numeric
    : std::integral_constant<
          bool, std::is_arithmetic_v<T> ||
                    (std::is_array_v<T> &&
                     std::is_arithmetic_v<typename std::remove_extent_t<T>>)> {
};

// Helper to format numbers with consistent precision
template <typename T>
std::string format_number(const T &value, int precision = 4) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

// Helper to calculate statistics for numeric series
template <typename T> struct SerieStats {
    T min;
    T max;
    double mean;
    double median;
    double q1;
    double q3;
    size_t num_zeros;
    size_t num_negative;
    size_t num_positive;
    size_t num_nan;

    static SerieStats compute(const Serie<T> &serie) {
        SerieStats stats;

        if (serie.empty()) {
            return stats;
        }

        // Calculate basic bounds
        auto [min_val, max_val] = bounds(serie);
        stats.min = min_val;
        stats.max = max_val;

        // Calculate mean
        stats.mean = avg(serie);

        // Calculate quantiles
        stats.q1 = quantile(serie, 0.25);
        stats.median = quantile(serie, 0.50);
        stats.q3 = quantile(serie, 0.75);

        // Count special values
        stats.num_zeros = 0;
        stats.num_negative = 0;
        stats.num_positive = 0;
        stats.num_nan = 0;

        serie.forEach([&](const T &val) {
            if (std::isnan(static_cast<double>(val))) {
                stats.num_nan++;
            } else if (val == 0) {
                stats.num_zeros++;
            } else if (val < 0) {
                stats.num_negative++;
            } else {
                stats.num_positive++;
            }
        });

        return stats;
    }
};
} // namespace detail

void Dataframe::dump(std::ostream &os, size_t max_preview) const {
    const int WIDTH_NAME = 20;
    const int WIDTH_TYPE = 15;
    const int WIDTH_SIZE = 10;
    const int WIDTH_STATS = 15;

    // Header
    os << "\n" << std::string(50, '=') << "\n";
    os << "Dataframe Summary (" << size() << " series)\n";
    os << std::string(50, '=') << "\n\n";

    // Series Overview Table
    printSeriesOverview(os, WIDTH_NAME, WIDTH_TYPE, WIDTH_SIZE);

    // Detailed Series Information
    os << "Detailed Series Information\n\n";
    // os << std::string(100, '-') << "\n";

    for (const auto &name : names()) {
        printSerieDetails(os, name, max_preview, WIDTH_STATS);
    }
}

void Dataframe::printSeriesOverview(std::ostream &os, int w_name, int w_type,
                                    int w_size) const {
    // Print header
    os << std::left << std::setw(w_name) << "Serie Name" << std::setw(w_type)
       << "Type" << std::setw(w_size) << "Size"
       << "Memory (bytes)\n";
    os << std::string(w_name + w_type + w_size + 15, '-') << "\n";

    // Print each serie's basic info
    for (const auto &name : names()) {
        const auto &serie_info = series_.at(name);
        os << std::left << std::setw(w_name) << name << std::setw(w_type)
           << type_name(name) << std::setw(w_size) << serie_info.data->size()
           << (serie_info.data->size() * sizeof(double)) << "\n";
    }
    os << "\n";
}

void Dataframe::printSerieDetails(std::ostream &os, const std::string &name,
                                  size_t max_preview, int w_stats) const {
    os << "***************************************\n";
    os << "Serie: " << name << "\n";
    os << "***************************************\n";
    const auto &serie_info = series_.at(name);

    // Preview values
    os << "Preview (" << std::min(serie_info.data->size(), max_preview)
       << " of " << serie_info.data->size() << " values):\n";

    try {
        printSeriePreview(os, name, max_preview);

        // Print statistics for numeric types
        const std::string type = type_name(name);
        // std::cout << "===> type: " << type << std::endl;
        if (type == "double") {
            printNumericStats<double>(os, name, w_stats);
        } else if (type == "int") {
            printNumericStats<int>(os, name, w_stats);
        } else if (type == "float") {
            printNumericStats<float>(os, name, w_stats);
        }
        // else if (type.find("array")!=std::string::npos) {
        //     os << "(array values)\n";
        // } else if (type.find("vector")!=std::string::npos) {
        //     os << "(vector values)\n";
        // }
    } catch (const std::exception &e) {
        os << "(Error previewing values: " << e.what() << ")\n";
    }

    // os << "==========================================================\n";
    os << "\n";
}

template <typename T>
void Dataframe::printNumericStats(std::ostream &os, const std::string &name,
                                  int width) const {
    const auto &serie = get<T>(name);
    auto stats = detail::SerieStats<T>::compute(serie);

    os << "\nStatistics:\n";
    os << std::left << std::setw(width)
       << "Min: " << detail::format_number(stats.min) << "\n";
    os << std::left << std::setw(width)
       << "Max: " << detail::format_number(stats.max) << "\n";
    os << std::left << std::setw(width)
       << "Mean: " << detail::format_number(stats.mean) << "\n";
    os << std::left << std::setw(width)
       << "Median: " << detail::format_number(stats.median) << "\n";
    os << std::left << std::setw(width)
       << "Q1 (25%): " << detail::format_number(stats.q1) << "\n";
    os << std::left << std::setw(width)
       << "Q3 (75%): " << detail::format_number(stats.q3) << "\n";
    os << std::left << std::setw(width)
       << "IQR: " << detail::format_number(stats.q3 - stats.q1) << "\n";

    os << "\nValue Counts:\n";
    os << std::left << std::setw(width) << "Zeros: " << stats.num_zeros << "\n";
    os << std::left << std::setw(width) << "Negative: " << stats.num_negative
       << "\n";
    os << std::left << std::setw(width) << "Positive: " << stats.num_positive
       << "\n";
    os << std::left << std::setw(width) << "NaN: " << stats.num_nan << "\n";
}

void Dataframe::printSeriePreview(std::ostream &os, const std::string &name,
                                  size_t max_preview) const {
    const auto &serie_info = series_.at(name);
    size_t preview_size = std::min(serie_info.data->size(), max_preview);

    if (type_name(name) == "double") {
        printNumericPreview<double>(os, name, preview_size);
    } else if (type_name(name) == "float") {
        printNumericPreview<float>(os, name, preview_size);
    } else if (type_name(name) == "int") {
        printNumericPreview<int>(os, name, preview_size);
    } else if (type_name(name) == "std::string") {
        const auto &serie = get<std::string>(name);
        for (size_t i = 0; i < preview_size; ++i) {
            os << "\"" << serie[i] << "\"";
            if (i < preview_size - 1)
                os << ", ";
        }
    } else if (type_name(name).find("vector") != std::string::npos) {
        os << "(vector values)";
    } else if (type_name(name).find("array") != std::string::npos) {
        os << "(array values)";
    }
    os << "\n";
}

template <typename T>
void Dataframe::printNumericPreview(std::ostream &os, const std::string &name,
                                    size_t preview_size) const {
    const auto &serie = get<T>(name);
    for (size_t i = 0; i < preview_size; ++i) {
        os << detail::format_number(serie[i]);
        if (i < preview_size - 1)
            os << ", ";
    }
}

} // namespace df