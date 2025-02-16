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
 * @brief Options for CSV reading/writing
 */
struct CSVOptions {
    char delimiter = ',';
    char quote_char = '"';
    bool has_header = true;
    bool trim_whitespace = true;
    bool skip_empty_lines = true;
    std::string null_value = "NA";
    size_t skip_rows = 0;
};

/**
 * @brief Read a CSV file into a Dataframe
 * @param filename Path to the CSV file
 * @param options CSV parsing options
 * @return Dataframe containing the CSV data
 * @throws std::runtime_error if file cannot be opened or parsing fails
 */
Dataframe read_csv(const std::string &filename, const CSVOptions &options = {});

/**
 * @brief Write a Dataframe to a CSV file
 * @param df Dataframe to write
 * @param filename Output file path
 * @param options CSV writing options
 * @throws std::runtime_error if file cannot be opened or writing fails
 */
void write_csv(const Dataframe &df, const std::string &filename,
               const CSVOptions &options = {});

} // namespace io
} // namespace df

#include "inline/csv.hxx"