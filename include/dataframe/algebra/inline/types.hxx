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

namespace df {

    template <typename T, size_t N> FullMatrix<T, N> FullMatrix<T, N>::Identity()
    {
        FullMatrix<T, N> I;
        for (std::size_t i = 0; i < N; ++i)
            I(i, i) = T(1);
        return I;
    }

    template <typename T, size_t N>
    inline FullMatrix<T, N>::FullMatrix()
        : mat_ {}
    {
    }

    template <typename T, size_t N>
    inline FullMatrix<T, N>::FullMatrix(const type& t)
        : mat_(t)
    {
    }

    // Initialize with nested initializer list
    template <typename T, size_t N>
    inline FullMatrix<T, N>::FullMatrix(std::initializer_list<std::initializer_list<T>> init)
    {
        size_t i = 0, j = 0;
        for (const auto& row : init) {
            for (const auto& val : row) {
                if (i < N && j < N) {
                    (*this)(i, j) = val;
                }
                j++;
            }
            i++;
            j = 0;
        }
    }

    template <typename T, size_t N> inline T& FullMatrix<T, N>::operator()(size_t i, size_t j)
    {
        if (i >= N || j >= N)
            throw std::out_of_range("Matrix index out of bounds");
        return mat_[j * N + i];
    }

    template <typename T, size_t N>
    inline const T& FullMatrix<T, N>::operator()(size_t i, size_t j) const
    {
        if (i >= N || j >= N)
            throw std::out_of_range("Matrix index out of bounds");
        return mat_[j * N + i];
    }

    // Raw data access
    template <typename T, size_t N> inline FullMatrix<T, N>::type& FullMatrix<T, N>::data()
    {
        return mat_.data();
    }

    template <typename T, size_t N> inline const FullMatrix<T, N>::type& FullMatrix<T, N>::data() const
    {
        return mat_.data();
    }

    // Matrix operations
    template <typename T, size_t N>
    inline FullMatrix<T, N> FullMatrix<T, N>::operator+(const FullMatrix<T, N>& other) const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N * N; ++i) {
            result.mat_[i] = mat_[i] + other.mat_[i];
        }
        return result;
    }

    template <typename T, size_t N>
    inline FullMatrix<T, N> FullMatrix<T, N>::operator-(const FullMatrix<T, N>& other) const
    {
        FullMatrix result;
        for (size_t i = 0; i < N * N; ++i) {
            result.mat_[i] = mat_[i] - other.mat_[i];
        }
        return result;
    }

    template <typename T, size_t N>
    inline FullMatrix<T, N> FullMatrix<T, N>::operator*(const FullMatrix<T, N>& other) const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                T sum = T();
                for (size_t k = 0; k < N; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    template <typename T, size_t N>
    inline FullMatrix<T, N> FullMatrix<T, N>::operator*(T scalar) const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N * N; ++i) {
            result.mat_[i] = mat_[i] * scalar;
        }
        return result;
    }

    template <typename T, size_t N>
    inline Vector<T, N> FullMatrix<T, N>::operator*(const Vector<T, N>& vec) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            T sum = T();
            for (size_t j = 0; j < N; ++j) {
                sum += (*this)(i, j) * vec[j];
            }
            result[i] = sum;
        }
        return result;
    }

    // Transpose
    template <typename T, size_t N> inline FullMatrix<T, N> FullMatrix<T, N>::transpose() const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result(i, j) = (*this)(j, i);
            }
        }
        return result;
    }

    // Determinant (for 2x2 and 3x3 matrices)
    template <typename T, size_t N> inline T FullMatrix<T, N>::determinant() const
    {
        if constexpr (N == 2) {
            return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
        } else if constexpr (N == 3) {
            return (*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1))
                - (*this)(0, 1) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 0))
                + (*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0));
        } else {
            throw std::runtime_error("Determinant not implemented for this matrix size");
        }
    }

    // Inverse (for 2x2 and 3x3 matrices)
    template <typename T, size_t N> inline FullMatrix<T, N> FullMatrix<T, N>::inverse() const
    {
        T det = determinant();
        if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
            throw std::runtime_error("Matrix is singular");
        }

        FullMatrix<T, N> result;
        if constexpr (N == 2) {
            result(0, 0) = (*this)(1, 1) / det;
            result(0, 1) = -(*this)(0, 1) / det;
            result(1, 0) = -(*this)(1, 0) / det;
            result(1, 1) = (*this)(0, 0) / det;
        } else if constexpr (N == 3) {
            // Compute cofactor matrix
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < N; ++j) {
                    T cofactor = computeCofactor(i, j);
                    result(j, i) = cofactor / det; // Note: j,i for transpose
                }
            }
        } else {
            throw std::runtime_error("Inverse not implemented for this matrix size");
        }
        return result;
    }

    // Comparison operators
    template <typename T, size_t N>
    inline bool FullMatrix<T, N>::operator==(const FullMatrix<T, N>& other) const
    {
        return mat_ == other.mat_;
    }

    template <typename T, size_t N>
    inline bool FullMatrix<T, N>::operator!=(const FullMatrix<T, N>& other) const
    {
        return !(*this == other);
    }

    // Helper for 3x3 inverse
    template <typename T, size_t N>
    inline T FullMatrix<T, N>::computeCofactor(size_t row, size_t col) const
    {
        if constexpr (N == 3) {
            T minor = 0;
            size_t r[2], c[2];
            size_t idx = 0;

            // Get indices for the 2x2 minor
            for (size_t i = 0; i < N; ++i) {
                if (i != row) {
                    r[idx] = i;
                    idx++;
                }
            }
            idx = 0;
            for (size_t j = 0; j < N; ++j) {
                if (j != col) {
                    c[idx] = j;
                    idx++;
                }
            }

            // Calculate minor
            minor = (*this)(r[0], c[0]) * (*this)(r[1], c[1])
                - (*this)(r[0], c[1]) * (*this)(r[1], c[0]);

            // Apply sign
            return ((row + col) % 2 == 0 ? 1 : -1) * minor;
        }
        throw std::runtime_error("Cofactor computation not implemented for this matrix size");
    }

    template <typename T, size_t N> inline Vector<T, N> FullMatrix<T, N>::row(size_t i) const
    {
        if (i >= N)
            throw std::out_of_range("Row index out of bounds");

        Vector<T, N> result;
        for (size_t j = 0; j < N; ++j) {
            result[j] = (*this)(i, j);
        }
        return result;
    }

    template <typename T, size_t N> inline Vector<T, N> FullMatrix<T, N>::col(size_t j) const
    {
        if (j >= N)
            throw std::out_of_range("Column index out of bounds");

        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = (*this)(i, j);
        }
        return result;
    }

    // =================================================================

    template <typename T, size_t N>
    inline SymmetricMatrix<T, N>::SymmetricMatrix()
        : mat_ {}
    {
    }

    template <typename T, size_t N>
    inline SymmetricMatrix<T, N>::SymmetricMatrix(const type& t)
        : mat_(t)
    {
    }

    template <typename T, size_t N>
    inline SymmetricMatrix<T, N>::SymmetricMatrix(
        std::initializer_list<std::initializer_list<T>> init)
    {
        size_t i = 0;
        for (const auto& row : init) {
            size_t j = 0;
            for (const auto& val : row) {
                if (i <= j && i < N && j < N) { // Only store upper triangle
                    (*this)(i, j) = val;
                }
                j++;
            }
            i++;
        }
    }

    // Convert index from 2D to 1D for upper triangular storage
    template <typename T, size_t N> inline size_t SymmetricMatrix<T, N>::index(size_t i, size_t j)
    {
        if (i > j)
            std::swap(i, j); // Ensure we're in upper triangle
        return (j * (j + 1)) / 2 + i;
    }

    // Element access (const and non-const)
    template <typename T, size_t N> inline T& SymmetricMatrix<T, N>::operator()(size_t i, size_t j)
    {
        if (i >= N || j >= N)
            throw std::out_of_range("Matrix index out of bounds");
        return mat_[index(i, j)];
    }

    template <typename T, size_t N>
    inline const T& SymmetricMatrix<T, N>::operator()(size_t i, size_t j) const
    {
        if (i >= N || j >= N)
            throw std::out_of_range("Matrix index out of bounds");
        return mat_[index(i, j)];
    }

    // Raw data access
    template <typename T, size_t N> inline SymmetricMatrix<T, N>::type& SymmetricMatrix<T, N>::data() { return mat_.data(); }

    template <typename T, size_t N> const inline SymmetricMatrix<T, N>::type& SymmetricMatrix<T, N>::data() const
    {
        return mat_.data();
    }

    // Matrix operations
    template <typename T, size_t N>
    inline SymmetricMatrix<T, N> SymmetricMatrix<T, N>::operator+(
        const SymmetricMatrix<T, N>& other) const
    {
        SymmetricMatrix<T, N> result;
        for (size_t i = 0; i < storage_size; ++i) {
            result.mat_[i] = mat_[i] + other.mat_[i];
        }
        return result;
    }

    template <typename T, size_t N>
    inline SymmetricMatrix<T, N> SymmetricMatrix<T, N>::operator-(
        const SymmetricMatrix<T, N>& other) const
    {
        SymmetricMatrix<T, N> result;
        for (size_t i = 0; i < storage_size; ++i) {
            result.mat_[i] = mat_[i] - other.mat_[i];
        }
        return result;
    }

    template <typename T, size_t N>
    inline SymmetricMatrix<T, N> SymmetricMatrix<T, N>::operator*(T scalar) const
    {
        SymmetricMatrix<T, N> result;
        for (size_t i = 0; i < storage_size; ++i) {
            result.mat_[i] = mat_[i] * scalar;
        }
        return result;
    }

    // Matrix multiplication (result may not be symmetric!)
    template <typename T, size_t N>
    inline FullMatrix<T, N> SymmetricMatrix<T, N>::operator*(
        const SymmetricMatrix<T, N>& other) const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                T sum = T();
                for (size_t k = 0; k < N; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    template <typename T, size_t N>
    inline Vector<T, N> SymmetricMatrix<T, N>::operator*(const Vector<T, N>& vec) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            T sum = T();
            for (size_t j = 0; j < N; ++j) {
                sum += (*this)(i, j) * vec[j];
            }
            result[i] = sum;
        }
        return result;
    }

    // Determinant (for 2x2 and 3x3 matrices)
    template <typename T, size_t N> inline T SymmetricMatrix<T, N>::determinant() const
    {
        if constexpr (N == 2) {
            return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(0, 1);
        } else if constexpr (N == 3) {
            const T& a11 = (*this)(0, 0);
            const T& a12 = (*this)(0, 1);
            const T& a13 = (*this)(0, 2);
            const T& a22 = (*this)(1, 1);
            const T& a23 = (*this)(1, 2);
            const T& a33 = (*this)(2, 2);

            return a11 * (a22 * a33 - a23 * a23) - a12 * (a12 * a33 - a23 * a13)
                + a13 * (a12 * a23 - a22 * a13);
        } else {
            throw std::runtime_error("Determinant not implemented for this matrix size");
        }
    }

    // Inverse (for 2x2 and 3x3 matrices)
    template <typename T, size_t N>
    inline SymmetricMatrix<T, N> SymmetricMatrix<T, N>::inverse() const
    {
        T det = determinant();
        if (std::abs(det) < std::numeric_limits<T>::epsilon()) {
            throw std::runtime_error("Matrix is singular");
        }

        SymmetricMatrix<T, N> result;
        if constexpr (N == 2) {
            result(0, 0) = (*this)(1, 1) / det;
            result(0, 1) = -(*this)(0, 1) / det;
            result(1, 1) = (*this)(0, 0) / det;
        } else if constexpr (N == 3) {
            const T& a11 = (*this)(0, 0);
            const T& a12 = (*this)(0, 1);
            const T& a13 = (*this)(0, 2);
            const T& a22 = (*this)(1, 1);
            const T& a23 = (*this)(1, 2);
            const T& a33 = (*this)(2, 2);

            // Compute cofactor matrix
            result(0, 0) = (a22 * a33 - a23 * a23) / det;
            result(0, 1) = (a13 * a23 - a12 * a33) / det;
            result(0, 2) = (a12 * a23 - a13 * a22) / det;
            result(1, 1) = (a11 * a33 - a13 * a13) / det;
            result(1, 2) = (a13 * a12 - a11 * a23) / det;
            result(2, 2) = (a11 * a22 - a12 * a12) / det;
        } else {
            throw std::runtime_error("Inverse not implemented for this matrix size");
        }
        return result;
    }

    // Comparison operators
    template <typename T, size_t N>
    inline bool SymmetricMatrix<T, N>::operator==(const SymmetricMatrix<T, N>& other) const
    {
        return mat_ == other.mat_;
    }

    template <typename T, size_t N>
    inline bool SymmetricMatrix<T, N>::operator!=(const SymmetricMatrix<T, N>& other) const
    {
        return !(*this == other);
    }

    template <typename T, size_t N> inline Vector<T, N> SymmetricMatrix<T, N>::row(size_t i) const
    {
        if (i >= N)
            throw std::out_of_range("Row index out of bounds");

        Vector<T, N> result;
        for (size_t j = 0; j < N; ++j) {
            result[j] = (*this)(i, j);
        }
        return result;
    }

    template <typename T, size_t N> inline Vector<T, N> SymmetricMatrix<T, N>::col(size_t j) const
    {
        if (j >= N)
            throw std::out_of_range("Column index out of bounds");

        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = (*this)(i, j);
        }
        return result;
    }

    // ----------------------------------------------------

    template <typename T, size_t N>
    Vector<T, N>::Vector()
        : data_ {}
    {
    }

    template <typename T, size_t N>
    Vector<T, N>::Vector(const type& t)
        : data_(t)
    {
    }

    template <typename T, size_t N> Vector<T, N>::Vector(std::initializer_list<T> init)
    {
        size_t i = 0;
        for (const auto& val : init) {
            if (i < N)
                data_[i] = val;
            i++;
        }
    }

    // Element access
    template <typename T, size_t N> T& Vector<T, N>::operator[](size_t i)
    {
        if (i >= N)
            throw std::out_of_range("Vector index out of bounds");
        return data_[i];
    }

    template <typename T, size_t N> const T& Vector<T, N>::operator[](size_t i) const
    {
        if (i >= N)
            throw std::out_of_range("Vector index out of bounds");
        return data_[i];
    }

    // Raw data access
    template <typename T, size_t N> T* Vector<T, N>::data() { return data_.data(); }

    template <typename T, size_t N> const T* Vector<T, N>::data() const { return data_.data(); }

    // Vector operations
    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator+(const Vector<T, N>& other) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data_[i] + other[i];
        }
        return result;
    }

    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator-(const Vector<T, N>& other) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data_[i] - other[i];
        }
        return result;
    }

    template <typename T, size_t N> Vector<T, N> Vector<T, N>::operator*(T scalar) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data_[i] * scalar;
        }
        return result;
    }

    // Dot product
    template <typename T, size_t N> T Vector<T, N>::dot(const Vector<T, N>& other) const
    {
        T result = T();
        for (size_t i = 0; i < N; ++i) {
            result += data_[i] * other[i];
        }
        return result;
    }

    // Cross product (only for 3D vectors)
    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::cross(const Vector<T, N>& other) const
    {
        static_assert(N == 3, "Cross product is only defined for 3D vectors");
        return Vector<T, N> { { data_[1] * other[2] - data_[2] * other[1],
            data_[2] * other[0] - data_[0] * other[2],
            data_[0] * other[1] - data_[1] * other[0] } };
    }

    template <typename T, size_t N> T Vector<T, N>::normSquared() const { return this->dot(*this); }

    template <typename T, size_t N> T Vector<T, N>::norm() const
    {
        return std::sqrt(normSquared());
    }

    template <typename T, size_t N> Vector<T, N> Vector<T, N>::normalized() const
    {
        T n = norm();
        if (n < std::numeric_limits<T>::epsilon()) {
            throw std::runtime_error("Cannot normalize zero vector");
        }
        return *this * (T(1) / n);
    }

    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator*(const FullMatrix<T, N>& mat) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            T sum = T();
            for (size_t j = 0; j < N; ++j) {
                sum += data_[j] * mat(j, i);
            }
            result[i] = sum;
        }
        return result;
    }

    template <typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator*(const SymmetricMatrix<T, N>& mat) const
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            T sum = T();
            for (size_t j = 0; j < N; ++j) {
                sum += data_[j] * mat(j, i);
            }
            result[i] = sum;
        }
        return result;
    }

    template <typename T, size_t N>
    FullMatrix<T, N> Vector<T, N>::tensor(const Vector<T, N>& other) const
    {
        FullMatrix<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result(i, j) = data_[i] * other[j];
            }
        }
        return result;
    }

    template <typename T, size_t N> bool Vector<T, N>::operator==(const Vector<T, N>& other) const
    {
        return data_ == other.data_;
    }

    template <typename T, size_t N> bool Vector<T, N>::operator!=(const Vector<T, N>& other) const
    {
        return !(*this == other);
    }

    template <typename T, size_t N> Vector<T, N> Vector<T, N>::zero() { return Vector<T, N>(); }

    template <typename T, size_t N> Vector<T, N> Vector<T, N>::ones()
    {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = T(1);
        }
        return result;
    }

    template <typename T, size_t N> Vector<T, N> Vector<T, N>::unit(size_t dir)
    {
        if (dir >= N)
            throw std::out_of_range("Invalid direction for unit vector");
        Vector<T, N> result;
        result[dir] = T(1);
        return result;
    }

} // namespace df

// Stream output
template <typename T, size_t N>
inline std::ostream& operator<<(std::ostream& os, const df::FullMatrix<T, N>& mat)
{
    static const double default_epsilon = 1e-12;
    for (size_t i = 0; i < N; ++i) {
        os << "[";
        for (size_t j = 0; j < N; ++j) {
            T val = mat(i, j);
            if (std::abs(val) < default_epsilon) {
                os << "0";
            } else {
                os << val;
            }
            if (j < N - 1)
                os << ", ";
        }
        os << "]\n";
    }
    return os;
}

// Stream output
template <typename T, size_t N>
inline std::ostream& operator<<(std::ostream& os, const df::SymmetricMatrix<T, N>& mat)
{
    static const double default_epsilon = 1e-12;
    for (size_t i = 0; i < N; ++i) {
        os << "[";
        for (size_t j = 0; j < N; ++j) {
            T val = mat(i, j);
            if (std::abs(val) < default_epsilon) {
                os << "0";
            } else {
                os << val;
            }
            if (j < N - 1)
                os << ", ";
        }
        os << "]\n";
    }
    return os;
}

template <typename T, size_t N>
inline std::ostream& operator<<(std::ostream& os, const df::Vector<T, N>& vec)
{
    static const double default_epsilon = 1e-12;
    os << "[";
    for (size_t i = 0; i < N; ++i) {
        T val = vec[i];
        if (std::abs(val) < default_epsilon) {
            os << "0";
        } else {
            os << val;
        }
        if (i < N - 1)
            os << ", ";
    }
    os << "]";
    return os;
}