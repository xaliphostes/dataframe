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
#include <array>
#include <iostream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename T> using ArrayType = std::vector<T>;

using String = std::string;
using Strings = std::vector<String>;
using uint = unsigned int;
using ushort = unsigned short;

// using iVector2 = std::array<uint, 2>;
// using iVector3 = std::array<uint, 3>;
// using iVector4 = std::array<uint, 4>;
// using iVector6 = std::array<uint, 6>;
template <size_t N> using iVector = std::array<uint, N>;
template <size_t N> using Vector = std::array<double, N>;

/**
 * CRTP (Curiously Recurring Template Pattern) to avoid code duplication
 */
template <typename Derived, typename TYPE, size_t N> struct VectorBase {
    std::array<TYPE, N> data;

    VectorBase() = default;
    VectorBase(const std::array<TYPE, N>& arr)
        : data(arr)
    {
    }

    constexpr size_t size() const { return N; }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    TYPE& operator[](size_t i) { return data[i]; }
    const TYPE& operator[](size_t i) const { return data[i]; }

    Derived& operator+=(const Derived& other)
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] += other.data[i];
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator/=(TYPE scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] /= scalar;
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator-=(const Derived& other)
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] -= other.data[i];
        }
        return static_cast<Derived&>(*this);
    }

    Derived& operator*=(TYPE scalar)
    {
        for (size_t i = 0; i < N; ++i) {
            data[i] *= scalar;
        }
        return static_cast<Derived&>(*this);
    }

    Derived operator+(const Derived& other) const
    {
        Derived result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Derived operator-(const Derived& other) const
    {
        Derived result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] - other.data[i];
        }
        return result;
    }

    Derived operator*(TYPE scalar) const
    {
        Derived result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }

    Derived operator/(TYPE scalar) const
    {
        Derived result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] / scalar;
        }
        return result;
    }

    TYPE dot(const Derived& other) const
    {
        TYPE result = 0.0;
        for (size_t i = 0; i < N; ++i) {
            result += data[i] * other.data[i];
        }
        return result;
    }

    constexpr bool is3D() const { return N == 3; }
    Derived cross(const Derived& other) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D vectors.");
        return { data[1] * other.data[2] - data[2] * other.data[1],
            data[2] * other.data[0] - data[0] * other.data[2],
            data[0] * other.data[1] - data[1] * other.data[0] };
    }

    bool is_orthogonal(const Derived& other, double tol = 1e-10) const
    {
        return std::abs(this->dot(other)) < tol;
    }

    void normalize()
    {
        double len = std::sqrt(this->dot(static_cast<const Derived&>(*this)));
        if (len > 1e-10) {
            for (size_t i = 0; i < N; ++i) {
                data[i] /= len;
            }
        }
    }

    Derived normalized() const
    {
        double len = std::sqrt(this->dot(static_cast<const Derived&>(*this)));
        if (len > 1e-10) {
            Derived result;
            for (size_t i = 0; i < N; ++i) {
                result.data[i] = data[i] / len;
            }
            return result;
        }
        return static_cast<const Derived&>(*this);
    }

    double length() const { return std::sqrt(this->dot(static_cast<const Derived&>(*this))); }
};

struct Vector2 : VectorBase<Vector2, double, 2> {
    Vector2() = default;
    Vector2(const std::initializer_list<double>& init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

struct Vector3 : VectorBase<Vector3, double, 3> {
    Vector3() = default;
    Vector3(const std::initializer_list<double>& init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

struct Vector4 : VectorBase<Vector4, double, 4> {
    Vector4() = default;
    Vector4(const std::initializer_list<double>& init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

struct Vector6 : VectorBase<Vector6, double, 6> {
    Vector6() = default;
    Vector6(const std::initializer_list<double>& init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

// =============================================================

/**
 * CRTP
 */
template <typename Derived, size_t N> struct MatrixBase {
    std::array<double, N> data;

    MatrixBase() = default;
    MatrixBase(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
    MatrixBase(const std::array<double, 4>& arr)
        : data(arr)
    {
    }

    constexpr size_t size() const { return N; }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    double& operator[](size_t i) { return data[i]; }
    const double& operator[](size_t i) const { return data[i]; }

    bool is_symmetric(double tol = 1e-10) const
    {
        if constexpr (N == 3 || N == 6 || N == 10) { // already symmetric matrix
            return true;
        }

        if constexpr (N == 4) {
            return std::abs(data[1] - data[2]) < tol;
        } else if constexpr (N == 9) {
            return std::abs(data[1] - data[3]) < tol && std::abs(data[2] - data[6]) < tol
                && std::abs(data[5] - data[7]) < tol;
        } else if constexpr (N == 16) {
            return std::abs(data[1] - data[4]) < tol && std::abs(data[2] - data[8]) < tol
                && std::abs(data[3] - data[12]) < tol && std::abs(data[6] - data[9]) < tol
                && std::abs(data[7] - data[13]) < tol && std::abs(data[11] - data[14]) < tol;
        } else {
            return false;
        }
    }
};

// Full matrices
struct Matrix2D : MatrixBase<Matrix2D, 4> {
    Matrix2D() = default;
    Matrix2D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};
struct Matrix3D : MatrixBase<Matrix3D, 9> {
    Matrix3D() = default;
    Matrix3D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};
struct Matrix4D : MatrixBase<Matrix4D, 16> {
    Matrix4D() = default;
    Matrix4D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

// Symmetric matrices
struct SMatrix2D : MatrixBase<SMatrix2D, 3> {
    SMatrix2D() = default;
    SMatrix2D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};
struct SMatrix3D : MatrixBase<SMatrix3D, 6> {
    SMatrix3D() = default;
    SMatrix3D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};
struct SMatrix4D : MatrixBase<SMatrix4D, 10> {
    SMatrix4D() = default;
    SMatrix4D(std::initializer_list<double> init)
    {
        std::copy(init.begin(), init.end(), data.begin());
    }
};

// Stress/Strain tensors (type aliases)
using Strain2D = SMatrix2D;
using Strain3D = SMatrix3D;
using Stress2D = SMatrix2D;
using Stress3D = SMatrix3D;

namespace df {
    namespace detail {

        /**
         * @brief Traits class for dimensionality of points or vectors (2D or 3D)
         */
        template <size_t DIM> struct point_type { };

        template <> struct point_type<2> {
            using type = Vector2;
        };

        template <> struct point_type<3> {
            using type = Vector3;
        };

    } // namespace detail
} // namespace df

// ===== OUTPUT OPERATORS =====

// Helper function to print std::array (internal use)
template <class T, std::size_t N> void print_array(std::ostream& o, const std::array<T, N>& m)
{
    o << "[";
    for (size_t i = 0; i < N - 1; ++i) {
        o << m[i] << ",";
    }
    o << m[N - 1] << "]";
}

// Output operators for each specific type
std::ostream& operator<<(std::ostream& o, const Vector2& v)
{
    print_array(o, v.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Vector3& v)
{
    print_array(o, v.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Vector4& v)
{
    print_array(o, v.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Vector6& v)
{
    print_array(o, v.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Matrix2D& m)
{
    print_array(o, m.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Matrix3D& m)
{
    print_array(o, m.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const Matrix4D& m)
{
    print_array(o, m.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const SMatrix2D& m)
{
    print_array(o, m.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const SMatrix3D& m)
{
    print_array(o, m.data);
    return o;
}

std::ostream& operator<<(std::ostream& o, const SMatrix4D& m)
{
    print_array(o, m.data);
    return o;
}

// REMOVED: The ambiguous generic std::array operator
// This was causing the compiler ambiguity error