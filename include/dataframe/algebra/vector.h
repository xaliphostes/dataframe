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

template <typename T, size_t N> class FullMatrix;
template <typename T, size_t N> class SymmetricMatrix;

template <typename T, size_t N> class Vector {
  public:
    using type = std::array<T, N>;
    using value_type = T;
    using size_type = size_t;
    static constexpr size_type size = N;

    Vector();
    explicit Vector(const type &t);
    Vector(const Vector &other) = default;
    Vector(Vector &&other) noexcept = default;

    Vector(std::initializer_list<T> init);

    Vector &operator=(const Vector &other) = default;
    Vector &operator=(Vector &&other) noexcept = default;

    T &operator[](size_t i);

    const T &operator[](size_t i) const;

    T *data();
    const T *data() const;

    Vector operator+(const Vector &other) const;
    Vector operator-(const Vector &other) const;
    Vector operator*(T scalar) const;

    T dot(const Vector &other) const;
    Vector cross(const Vector &other) const;
    T normSquared() const;
    T norm() const;

    Vector normalized() const;

    Vector operator*(const FullMatrix<T, N> &mat) const;
    Vector operator*(const SymmetricMatrix<T, N> &mat) const;

    FullMatrix<T, N> tensor(const Vector &other) const;

    bool operator==(const Vector &other) const;
    bool operator!=(const Vector &other) const;

    static Vector zero();
    static Vector ones();
    static Vector unit(size_t dir);

  private:
    type data_;
};

} // namespace df

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &os, const df::Vector<T, N> &vec);

#include "inline/vector.hxx"