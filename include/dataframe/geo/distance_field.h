/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge...
 * [MIT License text as in other files]
 */

#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/utils/kdtree.h>
#include <dataframe/types.h>

namespace df {

/**
 * @brief Computes the distance field between points using KDTree optimization
 *
 * Uses KDTree for efficient nearest neighbor searches to compute minimum
 * distances from each point to a set of reference points. This implementation
 * is especially efficient for large datasets.
 *
 * @tparam DIM Dimension of the space (2 for Vector2, 3 for Vector3)
 * @param points Serie containing the points to compute distances from
 * @param reference_points Serie containing the reference points
 * @return Serie<double> containing the distances from each point to its nearest
 * reference point
 *
 * Example:
 * @code
 * // 2D example
 * Serie<Vector2> points{{0,0}, {1,1}, {2,2}};
 * Serie<Vector2> reference_points{{1,0}, {2,1}};
 * auto distances = distance_field<2>(points, reference_points);
 *
 * // 3D example
 * Serie<Vector3> points3d{...};
 * Serie<Vector3> refs3d{...};
 * auto distances3d = distance_field<3>(points3d, refs3d);
 * @endcode
 */
template <size_t DIM>
Serie<double> distance_field(
    const Serie<typename detail::point_type<DIM>::type> &points,
    const Serie<typename detail::point_type<DIM>::type> &reference_points);

/**
 * @brief Bind function for use in pipelines
 */
template <size_t DIM>
auto bind_distance_field(
    const Serie<typename detail::point_type<DIM>::type> &reference_points);

} // namespace df

#include "inline/distance_field.hxx"