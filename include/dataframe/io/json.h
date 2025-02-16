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
 * @brief Read a JSON file into a Dataframe
 * @param filename Path to the JSON file
 * @return Dataframe containing the JSON data
 * @throws std::runtime_error if file cannot be opened or parsing fails
 */
Dataframe read_json(const std::string &filename);

/**
 * @brief Write a Dataframe to a JSON file
 * @param df Dataframe to write
 * @param filename Output file path
 * @param pretty Whether to format the JSON with indentation
 * @throws std::runtime_error if file cannot be opened or writing fails
 */
void write_json(const Dataframe &df, const std::string &filename,
                bool pretty = true);

} // namespace io
} // namespace df

#include "inline/json.hxx"