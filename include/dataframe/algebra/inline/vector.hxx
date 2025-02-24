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

template <typename T, size_t N> Vector<T, N>::Vector() : data_{} {}

template <typename T, size_t N>
Vector<T, N>::Vector(const type &t) : data_(t) {}

template <typename T, size_t N>
Vector<T, N>::Vector(std::initializer_list<T> init) {
    size_t i = 0;
    for (const auto &val : init) {
        if (i < N)
            data_[i] = val;
        i++;
    }
}

// Element access
template <typename T, size_t N> T &Vector<T, N>::operator[](size_t i) {
    if (i >= N)
        throw std::out_of_range("Vector index out of bounds");
    return data_[i];
}

template <typename T, size_t N>
const T &Vector<T, N>::operator[](size_t i) const {
    if (i >= N)
        throw std::out_of_range("Vector index out of bounds");
    return data_[i];
}

// Raw data access
template <typename T, size_t N> T *Vector<T, N>::data() { return data_.data(); }

template <typename T, size_t N> const T *Vector<T, N>::data() const {
    return data_.data();
}

// Vector operations
template <typename T, size_t N>
Vector<T, N> Vector<T, N>::operator+(const Vector<T, N> &other) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = data_[i] + other[i];
    }
    return result;
}

template <typename T, size_t N>
Vector<T, N> Vector<T, N>::operator-(const Vector<T, N> &other) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = data_[i] - other[i];
    }
    return result;
}

template <typename T, size_t N>
Vector<T, N> Vector<T, N>::operator*(T scalar) const {
    Vector<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = data_[i] * scalar;
    }
    return result;
}

// Dot product
template <typename T, size_t N>
T Vector<T, N>::dot(const Vector<T, N> &other) const {
    T result = T();
    for (size_t i = 0; i < N; ++i) {
        result += data_[i] * other[i];
    }
    return result;
}

// Cross product (only for 3D vectors)
template <typename T, size_t N>
Vector<T, N> Vector<T, N>::cross(const Vector<T, N> &other) const {
    static_assert(N == 3, "Cross product is only defined for 3D vectors");
    return Vector<T, N>{{data_[1] * other[2] - data_[2] * other[1],
                         data_[2] * other[0] - data_[0] * other[2],
                         data_[0] * other[1] - data_[1] * other[0]}};
}

template <typename T, size_t N> T Vector<T, N>::normSquared() const {
    return this->dot(*this);
}

template <typename T, size_t N> T Vector<T, N>::norm() const {
    return std::sqrt(normSquared());
}

template <typename T, size_t N> Vector<T, N> Vector<T, N>::normalized() const {
    T n = norm();
    if (n < std::numeric_limits<T>::epsilon()) {
        throw std::runtime_error("Cannot normalize zero vector");
    }
    return *this * (T(1) / n);
}

template <typename T, size_t N>
Vector<T, N> Vector<T, N>::operator*(const FullMatrix<T, N> &mat) const {
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
Vector<T, N> Vector<T, N>::operator*(const SymmetricMatrix<T, N> &mat) const {
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
FullMatrix<T, N> Vector<T, N>::tensor(const Vector<T, N> &other) const {
    FullMatrix<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result(i, j) = data_[i] * other[j];
        }
    }
    return result;
}

template <typename T, size_t N>
bool Vector<T, N>::operator==(const Vector<T, N> &other) const {
    return data_ == other.data_;
}

template <typename T, size_t N>
bool Vector<T, N>::operator!=(const Vector<T, N> &other) const {
    return !(*this == other);
}

template <typename T, size_t N> Vector<T, N> Vector<T, N>::zero() {
    return Vector<T, N>();
}

template <typename T, size_t N> Vector<T, N> Vector<T, N>::ones() {
    Vector<T, N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = T(1);
    }
    return result;
}

template <typename T, size_t N> Vector<T, N> Vector<T, N>::unit(size_t dir) {
    if (dir >= N)
        throw std::out_of_range("Invalid direction for unit vector");
    Vector<T, N> result;
    result[dir] = T(1);
    return result;
}

} // namespace df

template <typename T, size_t N>
inline std::ostream &operator<<(std::ostream &os, const df::Vector<T, N> &vec) {
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