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
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>

namespace df {
namespace io {

/**
 * @brief Options for controlling CSV reading and writing behavior
 *
 * This structure provides configuration options for CSV file operations.
 */
struct CSVOptions {
    char delimiter = ',';
    char quote_char = '"';
    bool has_header = true;
    bool all_double = false;
    bool trim_whitespace = true;
    bool skip_empty_lines = true;
    std::string null_value = "NA";
    size_t skip_rows = 0;
};

/**
 * @brief Reads a CSV file and converts it into a Dataframe
 *
 * This function reads a CSV file and automatically determines the appropriate
 * type for each column based on the data. It supports various options for
 * customizing the parsing behavior through the CSVOptions structure.
 *
 * Supported column types and their corresponding Serie types:
 * - Integer values -> Serie<int64_t>
 * - Floating-point values -> Serie<double>
 * - Text/Mixed values -> Serie<std::string>
 *
 * @param filename Path to the CSV file to read
 * @param options Configuration options for CSV parsing (optional)
 *
 * @return Dataframe containing the data from the CSV file
 *
 * @throws std::runtime_error if:
 *         - The file cannot be opened
 *         - CSV parsing fails
 *         - Column type detection fails
 *
 * @note Type detection is performed by analyzing all values in each column:
 *       - If all values can be parsed as integers -> int64_t
 *       - If all values can be parsed as numbers but some have decimals ->
 * double
 *       - Otherwise -> string
 *
 * @example
 * // Basic usage with default options
 * try {
 *     df::Dataframe data = df::io::read_csv("data.csv");
 *     auto numbers = data.get<int64_t>("number_column");
 *     auto names = data.get<std::string>("text_column");
 * } catch (const std::runtime_error& e) {
 *     std::cerr << "Error reading CSV: " << e.what() << std::endl;
 * }
 *
 * // Using custom options
 * df::io::CSVOptions options;
 * options.delimiter = ';';
 * options.quote_char = '\'';
 * options.has_header = false;
 * options.null_value = "NULL";
 * options.skip_rows = 2;
 *
 * df::Dataframe data = df::io::read_csv("data.csv", options);
 */
Dataframe read_csv(const std::string &filename, const CSVOptions &options = {});

/**
 * @brief Writes a Dataframe to a CSV file
 *
 * This function converts a Dataframe into a CSV file, with customizable
 * formatting options through the CSVOptions structure. Column names are written
 * as headers by default, and each Serie is converted to an appropriate string
 * representation.
 *
 * Supported Serie types and their CSV representations:
 * - Serie<int64_t> -> Plain integer text
 * - Serie<double> -> Decimal number text
 * - Serie<std::string> -> Text (quoted if contains delimiter, quotes, or
 * newlines)
 *
 * @param df The Dataframe to write
 * @param filename Path where the CSV file should be written
 * @param options Configuration options for CSV writing (optional)
 *
 * @throws std::runtime_error if:
 *         - The file cannot be opened for writing
 *         - Any Serie contains unsupported types
 *         - Writing operation fails
 *
 * @note
 * - Empty values in string Series are written as the specified null_value
 * - String values containing the delimiter, quotes, or newlines are
 * automatically quoted
 * - Quotes within string values are escaped by doubling them
 *
 * @example
 * // Create a sample dataframe
 * df::Dataframe data;
 * data.add("names", df::Serie<std::string>{"John", "Jane,Smith"});
 * data.add("ages", df::Serie<int64_t>{30, 25});
 * data.add("scores", df::Serie<double>{95.5, 98.7});
 *
 * // Basic writing with default options
 * try {
 *     df::io::write_csv(data, "output.csv");
 * } catch (const std::runtime_error& e) {
 *     std::cerr << "Error writing CSV: " << e.what() << std::endl;
 * }
 *
 * // Writing with custom options
 * df::io::CSVOptions options;
 * options.delimiter = ';';
 * options.null_value = "NULL";
 * options.has_header = false;
 *
 * df::io::write_csv(data, "output.csv", options);
 */
void write_csv(const Dataframe &df, const std::string &filename,
               const CSVOptions &options = {});

/**
 * @brief Writes a Dataframe to a CSV stream
 *
 * Overload of write_csv that writes to a stream instead of a file. This is
 * useful for writing to string streams, network streams, or other output
 * streams.
 *
 * @param df The Dataframe to write
 * @param os Output stream to write to
 * @param options Configuration options for CSV writing (optional)
 *
 * @see write_csv(const Dataframe&, const std::string&, const CSVOptions&)
 *
 * @example
 * df::Dataframe data;
 * // ... populate data ...
 *
 * // Write to string stream
 * std::stringstream ss;
 * df::io::write_csv(data, ss);
 * std::string csv_content = ss.str();
 *
 * // Write to standard output
 * df::io::write_csv(data, std::cout);
 */
void write_csv(const Dataframe &df, std::ostream &os,
               const CSVOptions &options = {});

} // namespace io
} // namespace df

#include "inline/csv.hxx"