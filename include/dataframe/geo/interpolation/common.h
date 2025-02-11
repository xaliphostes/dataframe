#pragma once
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief Calculate squared distance between two 2D points
 */
inline double distance_squared_2d(const Vector2 &p1, const Vector2 &p2) {
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    return dx * dx + dy * dy;
}

/**
 * @brief Calculate squared distance between two 3D points
 */
inline double distance_squared_3d(const Vector3 &p1, const Vector3 &p2) {
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    double dz = p1[2] - p2[2];
    return dx * dx + dy * dy + dz * dz;
}

} // namespace df