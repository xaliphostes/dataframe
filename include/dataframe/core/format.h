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

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <sstream>
#include <string>
#include <type_traits>

namespace df {

/**
 * @brief Concatenate a Serie with a prefix, separator, and suffix
 *
 * Creates a string representation of a Serie with customizable prefix,
 * separator, and suffix.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Serie to stringify
 * @param prefix String to prepend (default: "[")
 * @param separator String between elements (default: ", ")
 * @param suffix String to append (default: "]")
 * @return std::string The formatted string representation
 *
 * Example:
 * @code
 * Serie<double> values{1.1, 2.2, 3.3};
 *
 * // Default formatting
 * auto s1 = format(values);
 * // => "[1.1, 2.2, 3.3]"
 *
 * // Custom formatting
 * auto s2 = format(values, "{ ", "; ", " }");
 * // => "{ 1.1; 2.2; 3.3 }"
 * @endcode
 */
template <typename T>
std::string format(const Serie<T> &serie, const std::string &prefix = "[",
                   const std::string &separator = ", ",
                   const std::string &suffix = "]") {
    if (serie.empty()) {
        return prefix + suffix;
    }

    std::ostringstream oss;
    oss << prefix;

    // oss << format(serie, separator);
    for (size_t i = 0; i < serie.size() - 1; ++i) {
        oss << serie[i] << separator;
    }
    oss << serie[serie.size() - 1];

    oss << suffix;
    return oss.str();
}

/**
 * @brief Create a string concatenation operation for pipeline use
 *
 * Creates a function that can be used in a pipeline to concatenate
 * a value with other values.
 *
 * @tparam Args Types of objects to append
 * @param args Objects to append to the piped value
 * @return Function that concatenates its input with the provided args
 *
 * Example:
 * @code
 * auto name = "John";
 * auto greeting = name | bind_format(" says ", "hello!");
 * // => "John says hello!"
 *
 * // Multiple operations in a pipeline
 * auto result = "Count: "
 *   | bind_format(5)
 *   | bind_format(", Value: ", 3.14);
 * // => "Count: 5, Value: 3.14"
 * @endcode
 */
template <typename... Args> auto bind_format(const Args &...args) {
    return [args...](const auto &value) {
        // Convert the value to string if it's a char array (string literal)
        if constexpr (std::is_array_v<
                          std::remove_reference_t<decltype(value)>> &&
                      std::is_same_v<
                          std::remove_extent_t<
                              std::remove_reference_t<decltype(value)>>,
                          char>) {
            return concat(std::string(value), args...);
        } else {
            return concat(value, args...);
        }
    };
}

} // namespace df