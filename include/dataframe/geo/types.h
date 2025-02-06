#pragma once
#include <dataframe/Serie.h>
#include <map>

namespace df {

// using Attribute = Serie<double>;
// using Attributes = std::map<std::string, Attribute>;

using Positions2 = Serie<Vector2>;
using Positions3 = Serie<Vector3>;
using Segments = Serie<iVector2>;
using Triangles = Serie<iVector3>;

// Vector arithmetic operators
template <size_t N>
Vector<N> operator-(const Vector<N> &a, const Vector<N> &b) {
    Vector<N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = a[i] - b[i];
    }
    return result;
}

template <size_t N>
Vector<N> operator+(const Vector<N> &a, const Vector<N> &b) {
    Vector<N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

template <size_t N> Vector<N> operator*(const Vector<N> &v, double scalar) {
    Vector<N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = v[i] * scalar;
    }
    return result;
}

template <size_t N> Vector<N> operator*(double scalar, const Vector<N> &v) {
    return v * scalar;
}

template <size_t N> Vector<N> operator/(const Vector<N> &v, double scalar) {
    Vector<N> result;
    for (size_t i = 0; i < N; ++i) {
        result[i] = v[i] / scalar;
    }
    return result;
}

// Compound assignment operators
template <size_t N> Vector<N> &operator+=(Vector<N> &a, const Vector<N> &b) {
    for (size_t i = 0; i < N; ++i) {
        a[i] += b[i];
    }
    return a;
}

template <size_t N> Vector<N> &operator-=(Vector<N> &a, const Vector<N> &b) {
    for (size_t i = 0; i < N; ++i) {
        a[i] -= b[i];
    }
    return a;
}

template <size_t N> Vector<N> &operator*=(Vector<N> &v, double scalar) {
    for (size_t i = 0; i < N; ++i) {
        v[i] *= scalar;
    }
    return v;
}

template <size_t N> Vector<N> &operator/=(Vector<N> &v, double scalar) {
    for (size_t i = 0; i < N; ++i) {
        v[i] /= scalar;
    }
    return v;
}

// Utility functions
template <size_t N> double dot(const Vector<N> &a, const Vector<N> &b) {
    double result = 0;
    for (size_t i = 0; i < N; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

template <size_t N> double length(const Vector<N> &v) {
    return std::sqrt(dot(v, v));
}

template <size_t N> Vector<N> normalize(const Vector<N> &v) {
    double len = length(v);
    if (len > 1e-10) {
        return v / len;
    }
    return v;
}

// Cross product (only for 3D vectors)
inline Vector3 cross(const Vector3 &a, const Vector3 &b) {
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]};
}

// Check if tensor is symmetric
bool is_symmetric(const std::array<double, 9> &tensor, double tol = 1e-10) {
    return std::abs(tensor[1] - tensor[3]) < tol &&
           std::abs(tensor[2] - tensor[6]) < tol &&
           std::abs(tensor[5] - tensor[7]) < tol;
}

// Check if vectors are orthogonal
bool is_orthogonal(const Vector3 &v1, const Vector3 &v2, double tol = 1e-10) {
    double dot = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    return std::abs(dot) < tol;
}

} // namespace df
