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
#ifdef USE_CGAL
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Voronoi_diagram_2.h>
#endif
#include <dataframe/Serie.h>
#include <map>
#include <vector>

namespace df {

#ifdef USE_CGAL

/**
 * @brief Natural Neighbor interpolation for 2D points. This function uses the
 * CGAL library.
 *
 * This implementation uses the Sibson's method which is based on the ratio of
 * stolen areas when inserting a new point into the Voronoi diagram.
 *
 * The steps are:
 * 1. Create Delaunay triangulation of input points
 * 2. For each target point:
 *    - Find the natural neighbors (points whose Voronoi cells will be affected)
 *    - Calculate the areas of overlap between the new and old Voronoi cells
 *    - Use these areas as weights for interpolation
 */
template <typename T>
Serie<T> natural_neighbor_2d(const Serie<Vector2> &points,
                             const Serie<T> &values,
                             const Serie<Vector2> &targets);

#else

/**
 * @brief Natural neighbor interpolation. This function is empty as CGAL is not
 * used. Enable this feature by using `cmake -DUSE_CGAL=ON ..` in your
 * `build`directory.
 */
template <typename T>
Serie<T> natural_neighbor_2d(const Serie<Vector2> &, const Serie<T> &,
                             const Serie<Vector2> &);

#endif

} // namespace df

#include "inline/natural_neighbor.hxx"