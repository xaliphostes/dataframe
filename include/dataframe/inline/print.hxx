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

#include <dataframe/utils.h>
#include <dataframe/utils/zip.h>
#include <iomanip>
#include <sstream>
#include <tuple>

namespace df {

namespace detail {

// Helper to get maximum width needed for a value in a series
template <typename T> size_t get_value_width(const T &value, size_t precision) {
    std::ostringstream ss;
    ss.precision(precision);
    ss << std::fixed << value;
    return ss.str().length();
}

template <typename T>
size_t get_max_width(const Serie<T> &serie, size_t precision) {
    size_t max_width = 0;
    for (size_t i = 0; i < serie.size(); ++i) {
        max_width = std::max(max_width, get_value_width(serie[i], precision));
    }
    return max_width;
}

// Helper to print a horizontal separator line
inline void print_separator(const std::vector<size_t> &widths,
                            char separator = '-') {
    std::cout << "  "; // Initial indent
    for (size_t width : widths) {
        std::cout << '+' << std::string(width + 2, separator);
    }
    std::cout << '+' << std::endl;
}

// Helper to print a row header
inline void print_row_header(size_t row_idx, size_t max_row_header_width) {
    std::cout << "  " << std::setw(max_row_header_width) << std::right
              << row_idx << " ";
}

// -------------------------------------------------

// Helper to print tuple elements
template <size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
print_tuple(std::ostream &, const std::tuple<Tp...> &,
            const std::vector<size_t> &) {}

template <size_t I = 0, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...(Tp), void>::type
    print_tuple(std::ostream &out, const std::tuple<Tp...> &t,
                const std::vector<size_t> &widths) {
    out << "| " << std::setw(widths[I]) << std::get<I>(t) << " ";
    print_tuple<I + 1, Tp...>(out, t, widths);
}

template <size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
update_widths(const std::tuple<Tp...> &, std::vector<size_t> &, size_t) {}

template <size_t I = 0, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...(Tp), void>::type update_widths(const std::tuple<Tp...> &t,
                                               std::vector<size_t> &widths,
                                               size_t precision) {
    widths[I] = std::max(widths[I], get_value_width(std::get<I>(t), precision));
    update_widths<I + 1, Tp...>(t, widths, precision);
}

// Helper to get column headers for tuple elements
template <size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
print_headers(std::ostream &, const std::vector<size_t> &) {}

template <size_t I = 0, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...(Tp), void>::type
    print_headers(std::ostream &out, const std::vector<size_t> &widths) {
    out << "| " << std::setw(widths[I]) << std::left
        << "Serie " + std::to_string(I) << " ";
    print_headers<I + 1, Tp...>(out, widths);
}

} // namespace detail

// =============================================================

template <typename T>
void print(const std::vector<Serie<T>> &series, size_t precision) {
    if (series.empty()) {
        std::cout << "No series to display." << std::endl;
        return;
    }

    // Save stream state
    std::ios::fmtflags oldFlags = std::cout.flags();
    std::streamsize oldPrecision = std::cout.precision();

    // Set precision for floating point numbers
    std::cout.precision(precision);
    std::cout << std::fixed;

    // Get the size of each series and verify they're all the same
    size_t num_rows = series[0].size();
    for (const auto &serie : series) {
        if (serie.size() != num_rows) {
            std::cout << "Error: Series have different sizes" << std::endl;
            return;
        }
    }

    // Calculate maximum width for each series
    std::vector<size_t> column_widths;
    for (const auto &serie : series) {
        column_widths.push_back(detail::get_max_width(serie, precision));
    }

    // Calculate width needed for row indices
    size_t max_row_header_width = std::to_string(num_rows - 1).length();

    // Print header with series information
    std::cout << "\nDisplaying " << series.size() << " series of type "
              << type_name<T>() << " with " << num_rows << " elements each:\n"
              << std::endl;

    // Print column headers
    detail::print_separator(column_widths);
    std::cout << std::string(max_row_header_width + 3,
                             ' '); // Indent for row indices
    for (size_t i = 0; i < series.size(); ++i) {
        std::cout << "| " << std::setw(column_widths[i]) << std::left
                  << "Serie " + std::to_string(i) << " ";
    }
    std::cout << "|" << std::endl;
    detail::print_separator(column_widths);

    // Print data rows
    for (size_t row = 0; row < num_rows; ++row) {
        detail::print_row_header(row, max_row_header_width);

        for (size_t col = 0; col < series.size(); ++col) {
            std::cout << "| " /*<< std::setw(column_widths[col])*/
                      << series[col][row] << " ";
        }
        std::cout << "|" << std::endl;
    }

    // Print bottom separator
    detail::print_separator(column_widths);
    std::cout << std::endl;

    // Restore stream state
    std::cout.flags(oldFlags);
    std::cout.precision(oldPrecision);
}

// Maintain backwards compatibility with single series print
template <typename T> void print(const Serie<T> &serie, size_t precision) {
    print(std::vector<Serie<T>>{serie}, precision);
}

// Pipeline operation support
template <typename T> struct print_binder {
    size_t precision;
    explicit print_binder(size_t p = 4) : precision(p) {}

    void operator()(const Serie<T> &serie) const { print(serie, precision); }
};

template <typename T> auto bind_print(size_t precision) {
    return print_binder<T>(precision);
}

// --------------------------------------------------------

// Print function for zipped series
template <typename... Types>
void print(const Serie<std::tuple<Types...>> &zipped_serie, size_t precision) {
    if (zipped_serie.empty()) {
        std::cout << "Empty zipped series" << std::endl;
        return;
    }

    // Save stream state
    auto oldFlags = std::cout.flags();
    auto oldPrecision = std::cout.precision();

    // Set precision for floating point numbers
    std::cout.precision(precision);
    std::cout << std::fixed;

    // Calculate column widths
    std::vector<size_t> widths(sizeof...(Types), 0);
    for (const auto &tuple : zipped_serie.data()) {
        detail::update_widths(tuple, widths, precision);
    }

    // Calculate width for row indices
    size_t max_row_header_width =
        std::to_string(zipped_serie.size() - 1).length();

    // Print header
    std::cout << "\nDisplaying " << sizeof...(Types) << " zipped series with "
              << zipped_serie.size() << " elements:\n"
              << std::endl;

    // Print column headers
    detail::print_separator(widths);
    std::cout << std::string(max_row_header_width + 3, ' ');
    detail::print_headers<0, Types...>(std::cout, widths);
    std::cout << "|" << std::endl;
    detail::print_separator(widths);

    // Print data rows
    for (size_t i = 0; i < zipped_serie.size(); ++i) {
        std::cout << "  " << std::setw(max_row_header_width) << std::right << i
                  << " ";
        detail::print_tuple(std::cout, zipped_serie[i], widths);
        std::cout << "|" << std::endl;
    }

    detail::print_separator(widths);
    std::cout << std::endl;

    // Restore stream state
    std::cout.flags(oldFlags);
    std::cout.precision(oldPrecision);
}

// Pipeline operation support for zipped series
template <typename... Types> struct print_zip_binder {
    size_t precision;
    explicit print_zip_binder(size_t p = 4) : precision(p) {}

    void operator()(const Serie<std::tuple<Types...>> &serie) const {
        print(serie, precision);
    }
};

template <typename... Types> auto bind_print_zipped(size_t precision) {
    return print_zip_binder<Types...>(precision);
}

} // namespace df
