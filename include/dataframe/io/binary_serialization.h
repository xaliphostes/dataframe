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
#include <cstdint>
#include <dataframe/Serie.h>
#include <fstream>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace df {
namespace io {

// =============================================================================

// Endianness detection and conversion utilities
namespace detail {

// Type identifier hash function
size_t get_type_hash(const std::type_info &info);

// Header structure
struct FileHeader {
    uint32_t signature;      // File signature ("DFSR")
    uint32_t version;        // File format version
    uint32_t endian_check;   // Endianness check value
    uint64_t elements;       // Number of elements in the Serie
    uint8_t type_code;       // Type encoding
    uint64_t type_hash;      // Type hash code for type identification
    uint32_t type_name_size; // Size of type name string (if custom type)
    // Followed by type_name_size bytes of type name for custom types
};

// Detects system endianness
bool is_little_endian();

// Reverses byte order for different sized integer types
template <typename T> T swap_endian(T value);

// // Determine the type code for a given C++ type
// template <typename T> constexpr TypeCode get_type_code();

// Read data with potential endian conversion
template <typename T> T read_value(std::ifstream &ifs, bool swap_needed);

// Write data with potential endian conversion
template <typename T>
void write_value(std::ofstream &ofs, T value, bool swap_needed);

// Write standard array with potential endian conversion
template <typename T, size_t N>
void write_array(std::ofstream &ofs, const std::array<T, N> &arr,
                 bool swap_needed);

// Read standard array with potential endian conversion
template <typename T, size_t N>
std::array<T, N> read_array(std::ifstream &ifs, bool swap_needed);

// Type-specific serialization interface
template <typename T> struct serializer {
    static void write(std::ostream &ofs, const T &value, bool swap_needed);
    static T read(std::istream &ifs, bool swap_needed);
};

// Specialization for std::string
template <> struct serializer<std::string> {
    static void write(std::ostream &ofs, const std::string &value,
                      bool swap_needed);
    static std::string read(std::istream &ifs, bool swap_needed);
};

} // namespace detail

// =============================================================================

// Forward declaration of Serie factory
class SerieFactory;

/**
 * Global factory instance
 */
SerieFactory &getSerieFactory();

/**
 * @brief Serialize a Serie to a binary file
 *
 * Writes a Serie to a binary file with OS-independent format.
 * The format handles endianness differences and includes type information.
 *
 * @tparam T Type of the Serie elements
 * @param serie The Serie to serialize
 * @param filename The file path to write to
 * @return bool True if successful
 *
 * @throws std::runtime_error If the file cannot be opened or written
 */
template <typename T> bool save(const Serie<T> &serie, const std::string &);
template <typename T> bool save(const Serie<T> &serie, std::ostream &os);

/**
 * @brief Deserialize a Serie from a binary file with explicit type
 *
 * Reads a Serie from a binary file, automatically handling endianness
 * differences and validating type information.
 *
 * @tparam T Type of the Serie to create
 * @param filename The file path to read from
 * @return Serie<T> The deserialized Serie
 *
 * @throws std::runtime_error If the file cannot be opened or read
 * @throws std::runtime_error If the file format is invalid
 * @throws std::runtime_error If the file contains a different type than
 * requested
 */
template <typename T> Serie<T> load(const std::string &filename);
template <typename T> Serie<T> load(std::istream &is);

/**
 * @brief Deserialize a Serie from a binary file with automatic type detection
 *
 * Reads a Serie from a binary file, automatically detecting the type and
 * creating the appropriate Serie instance. Uses the SerieFactory system.
 *
 * @param filename The file path to read from
 * @return std::shared_ptr<SerieBase> A base pointer to the deserialized Serie
 *
 * @throws std::runtime_error If the file cannot be opened or read
 * @throws std::runtime_error If the file format is invalid
 * @throws std::runtime_error If the file contains a type that's not registered
 */
std::shared_ptr<SerieBase> load(const std::string &filename);
std::shared_ptr<SerieBase> load(std::istream &is);

/**
 * @brief Utility function to check what type a serialized file contains
 *
 * @param filename The file to check
 * @return std::string A string describing the type
 */
std::string get_file_type(const std::string &filename);
std::string get_file_type(std::istream &is);

/**
 * @brief Register a custom type with the serialization system
 *
 * This allows extending the serialization framework with custom types
 * by providing custom read/write functions.
 *
 * @tparam T The type to register
 * @param typeName A unique name for the type
 * @param writeFunc Function to write a value of type T to a stream
 * @param readFunc Function to read a value of type T from a stream
 */
template <typename T>
void registerCustomType(
    const std::string &typeName,
    std::function<void(std::ostream &, const T &, bool)> writeFunc,
    std::function<T(std::istream &, bool)> readFunc);

} // namespace io
} // namespace df

#include "inline/binary_serialization.hxx"