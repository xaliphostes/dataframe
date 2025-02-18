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
#include <fstream>
#include <string>

namespace df {
namespace io {

/**
 * @brief Reads a JSON file and converts it into a Dataframe
 *
 * This function reads a JSON file that contains an array of objects and
 * converts it into a Dataframe. Each object in the array becomes a row in the
 * Dataframe, with object keys becoming column names. The function automatically
 * determines the appropriate type for each column based on the data.
 *
 * Supported value types and their corresponding Serie types:
 * - Numbers without decimals -> Serie<int64_t>
 * - Numbers with decimals -> Serie<double>
 * - Strings -> Serie<std::string>
 * - Mixed or unsupported types -> Serie<std::string> (converted using
 * json.dump())
 *
 * @param filename Path to the JSON file to read
 *
 * @return Dataframe containing the data from the JSON file
 *
 * @throws std::runtime_error if:
 *         - The file cannot be opened
 *         - The JSON root is not an array
 *         - Any array element is not an object
 *         - JSON parsing fails
 *
 * @note The function expects the JSON to be structured as an array of objects
 * where each object has consistent key names. For example:
 *       [
 *         {"name": "John", "age": 30, "score": 95.5},
 *         {"name": "Jane", "age": 25, "score": 98.7}
 *       ]
 *
 * @example
 * try {
 *     df::Dataframe data = df::io::read_json("data.json");
 *     // Access the data
 *     auto names = data.get<std::string>("name");
 *     auto ages = data.get<int64_t>("age");
 *     auto scores = data.get<double>("score");
 * } catch (const std::runtime_error& e) {
 *     std::cerr << "Error reading JSON: " << e.what() << std::endl;
 * }
 */
Dataframe read_json(const std::string &filename);

/**
 * @brief Writes a Dataframe to a JSON file
 *
 * This function converts a Dataframe into a JSON array of objects and writes it
 * to a file. Each row of the Dataframe becomes an object in the JSON array,
 * with column names as keys.
 *
 * Supported Serie types and their JSON representations:
 * - Serie<int64_t> -> JSON number (integer)
 * - Serie<double> -> JSON number (floating-point)
 * - Serie<std::string> -> JSON string
 *
 * @param df The Dataframe to write
 * @param filename Path where the JSON file should be written
 * @param pretty Whether to format the JSON with indentation (default: true)
 *
 * @throws std::runtime_error if:
 *         - The file cannot be opened for writing
 *         - Any Serie contains unsupported types
 *
 * @note An empty Dataframe will be written as an empty JSON array ("[]")
 * @note If pretty=true, the JSON will be formatted with 4-space indentation
 *
 * @example
 * df::Dataframe data;
 * data.add("names", df::Serie<std::string>{"John", "Jane"});
 * data.add("ages", df::Serie<int64_t>{30, 25});
 * data.add("scores", df::Serie<double>{95.5, 98.7});
 *
 * try {
 *     // Write with pretty formatting
 *     df::io::write_json(data, "output.json", true);
 *
 *     // Write without formatting (minified)
 *     df::io::write_json(data, "output.min.json", false);
 * } catch (const std::runtime_error& e) {
 *     std::cerr << "Error writing JSON: " << e.what() << std::endl;
 * }
 */
void write_json(const Dataframe &, const std::string &, bool = true);

/**
 * @brief Writes a Dataframe to a JSON stream
 *
 * Overload of write_json that writes to a stream instead of a file. This is
 * useful for writing to string streams, network streams, or other output
 * streams.
 * 
 * @param df The Dataframe to write
 * @param filename Path where the CSV file should be written
 * @param options Configuration options for CSV writing (optional)
 */
void write_json(const Dataframe &df, std::ostream &os, bool = true);

} // namespace io
} // namespace df

#include "inline/json.hxx"