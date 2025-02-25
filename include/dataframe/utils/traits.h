

#pragma once
#include <array>
#include <type_traits>

namespace df {
namespace traits {

// Forward declarations of our trait classes
template <typename T> struct is_scalar;

template <typename T> struct is_vector;

template <typename T> struct is_matrix;

template <typename T> struct data_category;

// Category enum for classifying types
enum class DataCategory { Scalar, Vector, Matrix, Unknown };

// Base implementation for is_scalar
template <typename T> struct is_scalar {
    static constexpr bool value = std::is_arithmetic_v<T>;
};

// Base implementation for is_vector
template <typename T> struct is_vector : std::false_type {};

// Specialization for std::array
template <typename T, std::size_t N> struct is_vector<std::array<T, N>> {
    static constexpr bool value = is_scalar<T>::value;
};

// Base implementation for is_matrix
template <typename T> struct is_matrix : std::false_type {};

// Specialization for std::array of std::array
template <typename T, std::size_t N, std::size_t M>
struct is_matrix<std::array<std::array<T, M>, N>> {
    static constexpr bool value = is_scalar<T>::value;
};

// Helper variable templates for easier usage
template <typename T> inline constexpr bool is_scalar_v = is_scalar<T>::value;

template <typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T> inline constexpr bool is_matrix_v = is_matrix<T>::value;

// Data category determination
template <typename T> struct data_category {
    static constexpr DataCategory value =
        is_scalar_v<T>   ? DataCategory::Scalar
        : is_vector_v<T> ? DataCategory::Vector
        : is_matrix_v<T> ? DataCategory::Matrix
                         : DataCategory::Unknown;
};

template <typename T>
inline constexpr DataCategory data_category_v = data_category<T>::value;

// Helper functions for runtime checking
template <typename T> constexpr bool is_scalar_type() { return is_scalar_v<T>; }

template <typename T> constexpr bool is_vector_type() { return is_vector_v<T>; }

template <typename T> constexpr bool is_matrix_type() { return is_matrix_v<T>; }

template <typename T> constexpr DataCategory get_data_category() {
    return data_category_v<T>;
}

// Additional helper traits

// Get the element type of an array
template <typename T> struct array_element {
    using type = T;
};

template <typename T, std::size_t N> struct array_element<std::array<T, N>> {
    using type = T;
};

template <typename T> using array_element_t = typename array_element<T>::type;

// Get dimensions of array types
template <typename T> struct array_dimensions {
    static constexpr std::size_t value = 0;
};

template <typename T, std::size_t N> struct array_dimensions<std::array<T, N>> {
    static constexpr std::size_t value = 1;
};

template <typename T, std::size_t N, std::size_t M>
struct array_dimensions<std::array<std::array<T, M>, N>> {
    static constexpr std::size_t value = 2;
};

template <typename T>
inline constexpr std::size_t array_dimensions_v = array_dimensions<T>::value;

} // namespace traits
} // namespace df