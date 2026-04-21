#pragma once
#include <concepts>
#include <dataframe/Serie.h>
#include <map>

namespace df {

    using Positions2 = Serie<Vector2>;
    using Positions3 = Serie<Vector3>;
    using Segments = Serie<iVector2>;
    using Triangles = Serie<iVector3>;

    // --------------------------------------------------------

    // C++20 Concepts
    template <typename T>
    concept VectorType = std::same_as<T, Vector2> || std::same_as<T, Vector3>;

    // Free-function utilities (delegate to df::Vector members)

    template <typename T, size_t N>
    double dot(const Vector<T, N>& a, const Vector<T, N>& b)
    {
        return a.dot(b);
    }

    template <typename T, size_t N>
    double length(const Vector<T, N>& v) { return v.norm(); }

    template <typename T, size_t N>
    Vector<T, N> normalize(const Vector<T, N>& v)
    {
        T len = v.norm();
        if (len > T(1e-10)) {
            return v / len;
        }
        return v;
    }

    // Cross product (only for 3D vectors)
    template <typename T>
    Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b)
    {
        return a.cross(b);
    }

    // Check if tensor is symmetric
    inline bool is_symmetric(const std::array<double, 9>& tensor, double tol = 1e-10)
    {
        return std::abs(tensor[1] - tensor[3]) < tol && std::abs(tensor[2] - tensor[6]) < tol
            && std::abs(tensor[5] - tensor[7]) < tol;
    }

    // Check if vectors are orthogonal
    template <typename T, size_t N>
    bool is_orthogonal(const Vector<T, N>& v1, const Vector<T, N>& v2, double tol = 1e-10)
    {
        return std::abs(v1.dot(v2)) < tol;
    }

} // namespace df
