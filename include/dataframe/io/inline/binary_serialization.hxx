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

#include <algorithm>
#include <array>
#include <dataframe/utils/meta.h>
#include <typeindex>

namespace df {
namespace io {

// =========================================================================
// =========================================================================

namespace detail {

// Magic number for endian detection
constexpr uint32_t ENDIAN_MAGIC = 0x01020304;

// File format version
constexpr uint32_t CURRENT_VERSION = 2;

// File header signature ("DFSR" in ASCII)
constexpr uint32_t FILE_SIGNATURE = 0x44465352;

// Type encoding identifiers
enum class TypeCode : uint8_t {
    Bool = 1,
    Int8 = 2,
    Uint8 = 3,
    Int16 = 4,
    Uint16 = 5,
    Int32 = 6,
    Uint32 = 7,
    Int64 = 8,
    Uint64 = 9,
    Float = 10,
    Double = 11,
    String = 12,
    Custom = 255
};

// Helper to normalize type names (strip compiler-specific prefixes)
inline std::string normalize_type_name(const std::string &mangled_name) {
    // A very simple approach - find the first alphabetic character after any
    // digits
    size_t pos = 0;
    while (pos < mangled_name.size() && !std::isalpha(mangled_name[pos])) {
        pos++;
    }

    // Skip any digits after the first alphabetic character
    size_t start = pos;
    while (pos < mangled_name.size() && std::isdigit(mangled_name[pos])) {
        pos++;
    }

    // Return the rest of the string
    return mangled_name.substr(pos);
}

// Registry for custom type names
class TypeNameRegistry {
  public:
    template <typename T> static void registerName(const std::string &name) {
        getNames()[typeid(T).hash_code()] = name;
    }

    template <typename T> static std::string getName() {
        auto &names = getNames();
        auto hash = typeid(T).hash_code();
        auto it = names.find(hash);
        if (it != names.end()) {
            return it->second;
        }
        return typeid(T).name(); // Fall back to mangled name
    }

  private:
    static std::unordered_map<size_t, std::string> &getNames() {
        static std::unordered_map<size_t, std::string> names;
        return names;
    }
};

// Type identifier hash function
inline size_t get_type_hash(const std::type_info &info) {
    return info.hash_code();
}

// Detects system endianness
inline bool is_little_endian() {
    const uint32_t value = 1;
    return *reinterpret_cast<const uint8_t *>(&value) == 1;
}

// Reverses byte order for different sized integer types
template <typename T> inline T swap_endian(T value) {
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        return ((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8);
    } else if constexpr (sizeof(T) == 4) {
        return ((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) |
               ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24);
    } else if constexpr (sizeof(T) == 8) {
        return ((value & 0x00000000000000FF) << 56) |
               ((value & 0x000000000000FF00) << 40) |
               ((value & 0x0000000000FF0000) << 24) |
               ((value & 0x00000000FF000000) << 8) |
               ((value & 0x000000FF00000000) >> 8) |
               ((value & 0x0000FF0000000000) >> 24) |
               ((value & 0x00FF000000000000) >> 40) |
               ((value & 0xFF00000000000000) >> 56);
    } else {
        static_assert(sizeof(T) <= 8, "Unsupported type size for endian swap");
        return value;
    }
}

// Determine the type code for a given C++ type
template <typename T> constexpr TypeCode get_type_code() {
    if constexpr (std::is_same_v<T, bool>)
        return TypeCode::Bool;
    else if constexpr (std::is_same_v<T, int8_t>)
        return TypeCode::Int8;
    else if constexpr (std::is_same_v<T, uint8_t>)
        return TypeCode::Uint8;
    else if constexpr (std::is_same_v<T, int16_t>)
        return TypeCode::Int16;
    else if constexpr (std::is_same_v<T, uint16_t>)
        return TypeCode::Uint16;
    else if constexpr (std::is_same_v<T, int32_t>)
        return TypeCode::Int32;
    else if constexpr (std::is_same_v<T, uint32_t>)
        return TypeCode::Uint32;
    else if constexpr (std::is_same_v<T, int64_t>)
        return TypeCode::Int64;
    else if constexpr (std::is_same_v<T, uint64_t>)
        return TypeCode::Uint64;
    else if constexpr (std::is_same_v<T, float>)
        return TypeCode::Float;
    else if constexpr (std::is_same_v<T, double>)
        return TypeCode::Double;
    else if constexpr (std::is_same_v<T, std::string>)
        return TypeCode::String;
    else
        return TypeCode::Custom;
}

// Registry for custom type serializers
class SerializerRegistry {
  public:
    template <typename T> struct SerializerFunctions {
        std::function<void(std::ostream &, const T &, bool)> write_func;
        std::function<T(std::istream &, bool)> read_func;
        bool registered = false;
    };

    template <typename T> static SerializerFunctions<T> &get() {
        static SerializerFunctions<T> functions;
        return functions;
    }

    template <typename T>
    static void registerFunctions(
        std::function<void(std::ostream &, const T &, bool)> write_func,
        std::function<T(std::istream &, bool)> read_func) {
        auto &funcs = get<T>();
        funcs.write_func = write_func;
        funcs.read_func = read_func;
        funcs.registered = true;
    }

    template <typename T> static bool isRegistered() {
        return get<T>().registered;
    }
};

// Read data with potential endian conversion
template <typename T> inline T read_value(std::istream &ifs, bool swap_needed) {
    T value;
    ifs.read(reinterpret_cast<char *>(&value), sizeof(T));
    if (swap_needed && sizeof(T) > 1) {
        if constexpr (std::is_integral_v<T>) {
            value = swap_endian(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            // For floating-point types, we need to swap the underlying
            // representation
            if constexpr (sizeof(T) == 4) {
                uint32_t *as_int = reinterpret_cast<uint32_t *>(&value);
                *as_int = swap_endian(*as_int);
            } else if constexpr (sizeof(T) == 8) {
                uint64_t *as_int = reinterpret_cast<uint64_t *>(&value);
                *as_int = swap_endian(*as_int);
            }
        }
    }
    return value;
}

// Write data with potential endian conversion
template <typename T>
inline void write_value(std::ostream &ofs, T value, bool swap_needed) {
    if (swap_needed && sizeof(T) > 1) {
        if constexpr (std::is_integral_v<T>) {
            value = swap_endian(value);
        } else if constexpr (std::is_floating_point_v<T>) {
            // For floating-point types, we need to swap the underlying
            // representation
            if constexpr (sizeof(T) == 4) {
                uint32_t *as_int = reinterpret_cast<uint32_t *>(&value);
                *as_int = swap_endian(*as_int);
            } else if constexpr (sizeof(T) == 8) {
                uint64_t *as_int = reinterpret_cast<uint64_t *>(&value);
                *as_int = swap_endian(*as_int);
            }
        }
    }
    ofs.write(reinterpret_cast<const char *>(&value), sizeof(T));
}

// Write standard array with potential endian conversion
template <typename T, size_t N>
inline void write_array(std::ofstream &ofs, const std::array<T, N> &arr,
                        bool swap_needed) {
    for (size_t i = 0; i < N; ++i) {
        write_value(ofs, arr[i], swap_needed);
    }
}

// Read standard array with potential endian conversion
template <typename T, size_t N>
inline std::array<T, N> read_array(std::ifstream &ifs, bool swap_needed) {
    std::array<T, N> arr;
    for (size_t i = 0; i < N; ++i) {
        arr[i] = read_value<T>(ifs, swap_needed);
    }
    return arr;
}

// Type-specific serialization for complex types
template <typename T>
void serializer<T>::write(std::ostream &ofs, const T &value, bool swap_needed) {
    // First check if we have custom serializer registered
    if (SerializerRegistry::isRegistered<T>()) {
        SerializerRegistry::get<T>().write_func(ofs, value, swap_needed);
        return;
    }

    // Default implementation for non-registered types
    if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) {
        // For trivial types, direct binary write
        ofs.write(reinterpret_cast<const char *>(&value), sizeof(T));
    } else if constexpr (details::is_std_array_v<T>) {
        // For std::array
        constexpr size_t N = std::tuple_size_v<T>;
        using ElementType = typename T::value_type;
        for (size_t i = 0; i < N; ++i) {
            write_value<ElementType>(ofs, value[i], swap_needed);
        }
    } else {
        // For other types that don't have special handling
        throw std::runtime_error("Unsupported type for serialization: " +
                                 std::string(typeid(T).name()));
    }
}

template <typename T>
T serializer<T>::read(std::istream &is, bool swap_needed) {
    // First check if we have custom serializer registered
    if (SerializerRegistry::isRegistered<T>()) {
        return SerializerRegistry::get<T>().read_func(is, swap_needed);
    }

    // Default implementation for non-registered types
    if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) {
        // For trivial types, direct binary read
        T value;
        is.read(reinterpret_cast<char *>(&value), sizeof(T));
        return value;
    } else if constexpr (details::is_std_array_v<T>) {
        // For std::array
        constexpr size_t N = std::tuple_size_v<T>;
        using ElementType = typename T::value_type;
        T arr;
        for (size_t i = 0; i < N; ++i) {
            arr[i] = read_value<ElementType>(is, swap_needed);
        }
        return arr;
    } else {
        // For other types that don't have special handling
        throw std::runtime_error("Unsupported type for deserialization: " +
                                 std::string(typeid(T).name()));
    }
}

// Specialization for std::string
inline void serializer<std::string>::write(std::ostream &os,
                                           const std::string &value,
                                           bool swap_needed) {
    uint64_t size = value.size();
    write_value(os, size, swap_needed);
    os.write(value.data(), size);
}

inline std::string serializer<std::string>::read(std::istream &is,
                                                 bool swap_needed) {
    uint64_t size = read_value<uint64_t>(is, swap_needed);
    std::string value(size, '\0');
    is.read(&value[0], size);
    return value;
}

} // namespace detail

// =========================================================================

/**
 * @brief Type-erased creator interface for Serie instances
 *
 * Abstract base class for creating Serie objects of specific types
 */
class SerieCreator {
  public:
    virtual ~SerieCreator() = default;
    // Change from std::ifstream to std::istream
    virtual std::shared_ptr<SerieBase> create(std::istream &is,
                                              const detail::FileHeader &header,
                                              bool swap_needed) const = 0;
    virtual bool canHandle(const std::string &type_name,
                           uint64_t type_hash) const = 0;
    virtual std::unique_ptr<SerieCreator> clone() const = 0;
};

/**
 * @brief Typed implementation of SerieCreator
 *
 * Creates Serie instances of a specific type T
 */
template <typename T> class TypedSerieCreator : public SerieCreator {
  public:
    // Change std::ifstream to std::istream
    std::shared_ptr<SerieBase> create(std::istream &is,
                                      const detail::FileHeader &header,
                                      bool swap_needed) const override;
    bool canHandle(const std::string &type_name,
                   uint64_t type_hash) const override;
    std::unique_ptr<SerieCreator> clone() const override;
};

/**
 * @brief Custom serie creator with custom serializer functions
 *
 * Allows for registering custom types with their own
 * serialization/deserialization logic
 */
template <typename T> class CustomSerieCreator : public SerieCreator {
  public:
    // Change from std::ifstream to std::istream
    using ReadFunc = std::function<T(std::istream &, bool)>;

    CustomSerieCreator(const std::string &typeName, uint64_t typeHash,
                       ReadFunc readFunc);

    // Change std::ifstream to std::istream
    std::shared_ptr<SerieBase> create(std::istream &is,
                                      const detail::FileHeader &header,
                                      bool swap_needed) const override;

    bool canHandle(const std::string &type_name,
                   uint64_t type_hash) const override;

    std::unique_ptr<SerieCreator> clone() const override;

  private:
    std::string typeName_;
    uint64_t typeHash_;
    ReadFunc readFunc_;
};

/**
 * @brief Factory class for creating Serie instances from serialized data
 *
 * Manages registration of type creators and handles Serie instantiation
 * based on type information in the serialized data.
 */
class SerieFactory {
  public:
    SerieFactory();
    ~SerieFactory() = default;

    // Change from std::ifstream to std::istream
    std::shared_ptr<SerieBase> createSerie(std::istream &is,
                                           const detail::FileHeader &header,
                                           bool swap_needed,
                                           const std::string &type_name) const;

    template <typename T> void registerType();

    template <typename T>
    void registerCustomType(const std::string &typeName, uint64_t typeHash,
                            typename CustomSerieCreator<T>::ReadFunc readFunc);

  private:
    void registerBuiltinTypes();

    std::vector<std::unique_ptr<SerieCreator>> creators_;
};

// =========================================================================

template <typename T>
inline bool save(const Serie<T> &serie, const std::string &filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing: " +
                                 filename);
    }
    return save(serie, ofs);
}

template <typename T>
inline bool save(const Serie<T> &serie, std::ostream &os) {
    // Write file header
    detail::FileHeader header;
    header.signature = detail::FILE_SIGNATURE;
    header.version = detail::CURRENT_VERSION;
    header.endian_check = detail::ENDIAN_MAGIC;
    header.elements = serie.size();
    header.type_code = static_cast<uint8_t>(detail::get_type_code<T>());

    // Get the clean type name if registered, otherwise use the compiler's name
    std::string type_name = detail::TypeNameRegistry::getName<T>();
    header.type_hash = detail::get_type_hash(typeid(T));
    header.type_name_size = type_name.size();

    // Write header
    os.write(reinterpret_cast<const char *>(&header), sizeof(header));

    // Write type name
    os.write(type_name.data(), type_name.size());

    // Write data
    for (size_t i = 0; i < serie.size(); ++i) {
        detail::serializer<T>::write(os, serie[i], false);
    }

    return os.good();
}

template <typename T> Serie<T> inline load(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file for reading: " +
                                 filename);
    }
    return load<T>(ifs);
}

template <typename T> Serie<T> inline load(std::istream &is) {
    // Read header
    detail::FileHeader header;
    is.read(reinterpret_cast<char *>(&header), sizeof(header));

    // Verify signature
    if (header.signature != detail::FILE_SIGNATURE) {
        throw std::runtime_error("Invalid file format: incorrect signature");
    }

    // Check version
    if (header.version > detail::CURRENT_VERSION) {
        throw std::runtime_error(
            "File was created with a newer version of the library");
    }

    // Detect endianness
    bool swap_needed = false;
    bool is_system_little_endian = detail::is_little_endian();
    bool is_file_little_endian = (header.endian_check == detail::ENDIAN_MAGIC);

    if (is_file_little_endian != is_system_little_endian) {
        // Need to swap endianness
        swap_needed = true;

        // Need to fix the header values that we just read
        header.elements = detail::swap_endian(header.elements);
        header.type_hash = detail::swap_endian(header.type_hash);
        header.type_name_size = detail::swap_endian(header.type_name_size);
    }

    // Read type name
    std::string type_name;
    if (header.type_name_size > 0) {
        type_name.resize(header.type_name_size);
        is.read(&type_name[0], header.type_name_size);
    }

    // Type checking - using both type code and hash if available
    auto expected_type_code = detail::get_type_code<T>();
    if (header.type_code != static_cast<uint8_t>(expected_type_code) ||
        (header.version >= 2 &&
         header.type_hash != detail::get_type_hash(typeid(T)))) {
        std::string msg =
            "Type mismatch: file contains a different type than requested";
        if (!type_name.empty()) {
            msg += " (file type: " + type_name + ")";
        }
        throw std::runtime_error(msg);
    }

    // Read data
    std::vector<T> data;
    data.reserve(header.elements);

    for (size_t i = 0; i < header.elements; ++i) {
        data.push_back(detail::serializer<T>::read(is, swap_needed));
    }

    return Serie<T>(data);
}

inline std::shared_ptr<SerieBase> load(std::istream &is) {
    // Read header
    detail::FileHeader header;
    is.read(reinterpret_cast<char *>(&header), sizeof(header));

    // Verify signature
    if (header.signature != detail::FILE_SIGNATURE) {
        throw std::runtime_error("Invalid file format: incorrect signature");
    }

    // Check version
    if (header.version > detail::CURRENT_VERSION) {
        throw std::runtime_error(
            "File was created with a newer version of the library");
    }

    // Detect endianness
    bool swap_needed = false;
    bool is_system_little_endian = detail::is_little_endian();
    bool is_file_little_endian = (header.endian_check == detail::ENDIAN_MAGIC);

    if (is_file_little_endian != is_system_little_endian) {
        // Need to swap endianness
        swap_needed = true;

        // Need to fix the header values that we just read
        header.elements = detail::swap_endian(header.elements);
        header.type_hash = detail::swap_endian(header.type_hash);
        header.type_name_size = detail::swap_endian(header.type_name_size);
    }

    // Read type name
    std::string type_name;
    if (header.type_name_size > 0) {
        type_name.resize(header.type_name_size);
        is.read(&type_name[0], header.type_name_size);
    } else if (header.version >= 2) {
        // Version 2+ should always have a type name
        throw std::runtime_error(
            "Invalid file format: missing type information");
    }

    // Return to the factory for creating appropriate Serie type
    return getSerieFactory().createSerie(is, header, swap_needed, type_name);
}

inline std::shared_ptr<SerieBase> load(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file for reading: " +
                                 filename);
    }

    return load(ifs);
}

inline std::string get_file_type(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    return get_file_type(ifs);
}

inline std::string get_file_type(std::istream &is) {
    // Save the current position in the stream
    std::streampos original_pos = is.tellg();

    // Check if stream is in good state
    if (!is.good()) {
        throw std::runtime_error("Stream is not in a good state for reading");
    }

    // Read header
    detail::FileHeader header;
    is.read(reinterpret_cast<char *>(&header), sizeof(header));

    // Verify signature
    if (header.signature != detail::FILE_SIGNATURE) {
        // Restore original position before throwing
        is.seekg(original_pos);
        throw std::runtime_error("Invalid file format: not a valid Serie file");
    }

    // Check endianness
    bool is_system_little_endian = detail::is_little_endian();
    bool is_file_little_endian = (header.endian_check == detail::ENDIAN_MAGIC);
    bool swap_needed = (is_file_little_endian != is_system_little_endian);

    if (swap_needed) {
        header.type_name_size = detail::swap_endian(header.type_name_size);
    }

    // Determine type
    detail::TypeCode type_code =
        static_cast<detail::TypeCode>(header.type_code);

    // Read custom type name if present
    std::string type_name;
    if (header.type_name_size > 0) {
        type_name.resize(header.type_name_size);
        is.read(&type_name[0], header.type_name_size);
    }

    // Restore original position in the stream
    is.seekg(original_pos);

    // Map type code to string
    switch (type_code) {
    case detail::TypeCode::Bool:
        return "Bool";
    case detail::TypeCode::Int8:
        return "Int8";
    case detail::TypeCode::Uint8:
        return "Uint8";
    case detail::TypeCode::Int16:
        return "Int16";
    case detail::TypeCode::Uint16:
        return "Uint16";
    case detail::TypeCode::Int32:
        return "Int32";
    case detail::TypeCode::Uint32:
        return "Uint32";
    case detail::TypeCode::Int64:
        return "Int64";
    case detail::TypeCode::Uint64:
        return "Uint64";
    case detail::TypeCode::Float:
        return "Float";
    case detail::TypeCode::Double:
        return "Double";
    case detail::TypeCode::String:
        return "String";
    case detail::TypeCode::Custom:
        return "Custom: " + type_name;
    default:
        return "Unknown";
    }
}

// ======== TypedSerieCreator implementation ========
template <typename T>
std::shared_ptr<SerieBase>
TypedSerieCreator<T>::create(std::istream &is, const detail::FileHeader &header,
                             bool swap_needed) const {
    std::vector<T> data;
    data.reserve(header.elements);

    for (size_t i = 0; i < header.elements; ++i) {
        data.push_back(detail::serializer<T>::read(is, swap_needed));
    }

    return std::make_shared<Serie<T>>(data);
}

template <typename T>
bool TypedSerieCreator<T>::canHandle(const std::string &type_name,
                                     uint64_t type_hash) const {
    // First try exact match
    if (type_name == typeid(T).name() ||
        type_hash == detail::get_type_hash(typeid(T))) {
        return true;
    }

    // Try normalized names
    std::string normalized_input = detail::normalize_type_name(type_name);
    std::string normalized_this = detail::normalize_type_name(typeid(T).name());

    return normalized_input == normalized_this;
}

template <typename T>
std::unique_ptr<SerieCreator> TypedSerieCreator<T>::clone() const {
    return std::make_unique<TypedSerieCreator<T>>();
}

// ======== CustomSerieCreator implementation ========
template <typename T>
CustomSerieCreator<T>::CustomSerieCreator(const std::string &typeName,
                                          uint64_t typeHash, ReadFunc readFunc)
    : typeName_(typeName), typeHash_(typeHash), readFunc_(readFunc) {}

template <typename T>
std::shared_ptr<SerieBase>
CustomSerieCreator<T>::create(std::istream &is,
                              const detail::FileHeader &header,
                              bool swap_needed) const {
    std::vector<T> data;
    data.reserve(header.elements);

    for (size_t i = 0; i < header.elements; ++i) {
        data.push_back(readFunc_(is, swap_needed));
    }

    return std::make_shared<Serie<T>>(data);
}

template <typename T>
bool CustomSerieCreator<T>::canHandle(const std::string &type_name,
                                      uint64_t type_hash) const {
    // First try exact match
    if (type_name == typeName_ || type_hash == typeHash_) {
        return true;
    }

    // Try normalized names
    std::string normalized_input = detail::normalize_type_name(type_name);
    std::string normalized_this = detail::normalize_type_name(typeName_);

    return normalized_input == normalized_this;
}

template <typename T>
std::unique_ptr<SerieCreator> CustomSerieCreator<T>::clone() const {
    return std::make_unique<CustomSerieCreator<T>>(typeName_, typeHash_,
                                                   readFunc_);
}

// ======== SerieFactory implementation ========
inline SerieFactory::SerieFactory() {
    // Register built-in types
    registerBuiltinTypes();
}

inline std::shared_ptr<SerieBase>
SerieFactory::createSerie(std::istream &is, const detail::FileHeader &header,
                          bool swap_needed,
                          const std::string &type_name) const {
    // Find the appropriate creator based on type name or hash
    for (const auto &creator : creators_) {
        if (creator->canHandle(type_name, header.type_hash)) {
            return creator->create(is, header, swap_needed);
        }
    }

    throw std::runtime_error("Unknown type in serialized data: " + type_name);
}

template <typename T> void SerieFactory::registerType() {
    creators_.push_back(std::make_unique<TypedSerieCreator<T>>());
}

template <typename T>
void SerieFactory::registerCustomType(
    const std::string &typeName, uint64_t typeHash,
    typename CustomSerieCreator<T>::ReadFunc readFunc) {
    creators_.push_back(
        std::make_unique<CustomSerieCreator<T>>(typeName, typeHash, readFunc));
}

inline void SerieFactory::registerBuiltinTypes() {
    registerType<bool>();
    registerType<int8_t>();
    registerType<uint8_t>();
    registerType<int16_t>();
    registerType<uint16_t>();
    registerType<int32_t>();
    registerType<uint32_t>();
    registerType<int64_t>();
    registerType<uint64_t>();
    registerType<float>();
    registerType<double>();
    registerType<std::string>();
}

// Global factory instance
inline SerieFactory &getSerieFactory() {
    static SerieFactory factory;
    return factory;
}

// ======== Custom type registration ========

template <typename T>
inline void
registerCustomType(const std::string &typeName,
                   std::function<void(std::ostream &, const T &, bool)>
                       writeFunc, // Changed from std::ofstream to std::ostream
                   std::function<T(std::istream &, bool)>
                       readFunc) // Changed from std::ifstream to std::istream
{
    // Calculate a hash for the type
    uint64_t typeHash = std::hash<std::string>{}(typeName);

    // Register with the factory for automatic type detection
    getSerieFactory().registerCustomType<T>(typeName, typeHash, readFunc);

    // Register with the serializer registry
    detail::SerializerRegistry::registerFunctions<T>(writeFunc, readFunc);
}

} // namespace io
} // namespace df