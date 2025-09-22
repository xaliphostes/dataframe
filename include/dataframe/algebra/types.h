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
 */

#pragma once
#include <array>
#include <iostream>

namespace df {

    template <typename T, size_t N> class Vector;

    // =================================================================

    template <typename T, size_t N> class FullMatrix {
    public:
        using type = std::array<T, N * N>;
        using value_type = T;
        using size_type = size_t;
        static constexpr size_type size = N;

        static FullMatrix<T, N> Identity();

        FullMatrix();
        explicit FullMatrix(const type& t);
        FullMatrix(std::initializer_list<double> init)
        {
            std::copy(init.begin(), init.end(), mat_.begin());
        }
        FullMatrix(const FullMatrix& other) = default;
        FullMatrix(FullMatrix&& other) noexcept = default;

        FullMatrix(std::initializer_list<std::initializer_list<T>> init);

        FullMatrix& operator=(const FullMatrix& other) = default;
        FullMatrix& operator=(FullMatrix&& other) noexcept = default;

        T& operator()(size_t i, size_t j);
        const T& operator()(size_t i, size_t j) const;

        FullMatrix<T, N>::type& data();
        const FullMatrix<T, N>::type& data() const;

        FullMatrix operator+(const FullMatrix& other) const;
        FullMatrix operator-(const FullMatrix& other) const;
        FullMatrix operator*(const FullMatrix& other) const;
        FullMatrix operator*(T scalar) const;
        Vector<T, N> operator*(const Vector<T, N>& vec) const;

        FullMatrix transpose() const;
        T determinant() const;
        FullMatrix inverse() const;

        bool operator==(const FullMatrix& other) const;
        bool operator!=(const FullMatrix& other) const;

        Vector<T, N> row(size_t i) const;
        Vector<T, N> col(size_t j) const;

    private:
        type mat_;
        T computeCofactor(size_t row, size_t col) const;
    };

    // =================================================================

    // For an NxN symmetric matrix, we only need N*(N+1)/2 elements
    template <typename T, size_t N> class SymmetricMatrix {
    public:
        static constexpr size_t storage_size = (N * (N + 1)) / 2;
        using type = std::array<T, storage_size>;
        using value_type = T;
        using size_type = size_t;
        static constexpr size_type size = N;

        // Constructors
        SymmetricMatrix();
        explicit SymmetricMatrix(const type& t);
        SymmetricMatrix(std::initializer_list<double> init)
        {
            std::copy(init.begin(), init.end(), mat_.begin());
        }
        SymmetricMatrix(const SymmetricMatrix& other) = default;
        SymmetricMatrix(SymmetricMatrix&& other) noexcept = default;

        SymmetricMatrix(std::initializer_list<std::initializer_list<T>> init);

        SymmetricMatrix& operator=(const SymmetricMatrix& other) = default;
        SymmetricMatrix& operator=(SymmetricMatrix&& other) noexcept = default;

        static size_t index(size_t i, size_t j);

        T& operator()(size_t i, size_t j);
        const T& operator()(size_t i, size_t j) const;

        SymmetricMatrix<T, N>::type& data();
        const SymmetricMatrix<T, N>::type& data() const;

        SymmetricMatrix operator+(const SymmetricMatrix& other) const;
        SymmetricMatrix operator-(const SymmetricMatrix& other) const;
        SymmetricMatrix operator*(T scalar) const;
        FullMatrix<T, N> operator*(const SymmetricMatrix& other) const;
        Vector<T, N> operator*(const Vector<T, N>& vec) const;

        T determinant() const;
        SymmetricMatrix inverse() const;

        bool operator==(const SymmetricMatrix& other) const;
        bool operator!=(const SymmetricMatrix& other) const;

        Vector<T, N> row(size_t i) const;
        Vector<T, N> col(size_t j) const;

    private:
        type mat_; // Stores upper triangle in row-major order
    };

    // =================================================================

    template <typename T, size_t N> class Vector {
    public:
        using type = std::array<T, N>;
        using value_type = T;
        using size_type = size_t;
        // static constexpr size_type size = N;

        Vector();
        explicit Vector(const type& t);
        Vector(const Vector& other) = default;
        Vector(Vector&& other) noexcept = default;

        Vector(std::initializer_list<T> init);

        Vector& operator=(const Vector& other) = default;
        Vector& operator=(Vector&& other) noexcept = default;

        T& operator[](size_t i);

        const T& operator[](size_t i) const;

        T* data();
        const T* data() const;

        size_t size() const {return N;}

        Vector operator+(const Vector& other) const;
        Vector operator-(const Vector& other) const;
        Vector operator*(T scalar) const;

        T dot(const Vector& other) const;
        Vector cross(const Vector& other) const;
        T normSquared() const;
        T norm() const;

        Vector normalized() const;

        Vector operator*(const FullMatrix<T, N>& mat) const;
        Vector operator*(const SymmetricMatrix<T, N>& mat) const;

        FullMatrix<T, N> tensor(const Vector& other) const;

        bool operator==(const Vector& other) const;
        bool operator!=(const Vector& other) const;

        static Vector zero();
        static Vector ones();
        static Vector unit(size_t dir);

    private:
        type data_;
    };

    using Matrix2D = FullMatrix<double, 2>;
    using Matrix3D = FullMatrix<double, 3>;
    using Matrix4D = FullMatrix<double, 4>;

    using SMatrix2D = SymmetricMatrix<double, 2>;
    using SMatrix3D = SymmetricMatrix<double, 3>;
    using SMatrix4D = SymmetricMatrix<double, 4>;

    using Strain2D = SMatrix2D;
    using Stress2D = SMatrix2D;
    using Strain3D = SMatrix3D;
    using Stress3D = SMatrix3D;

    using Vector2D = Vector<double, 2>;
    using Vector3D = Vector<double, 3>;
    using Vector4D = Vector<double, 4>;

} // namespace df

template <typename T, size_t N>
std::ostream& operator<<(std::ostream&, const df::FullMatrix<T, N>&);

template <typename T, size_t N>
std::ostream& operator<<(std::ostream&, const df::SymmetricMatrix<T, N>&);

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const df::Vector<T, N>& vec);

#include "inline/types.hxx"