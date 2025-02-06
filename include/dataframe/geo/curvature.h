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
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>

namespace df {

/**
 * @brief Compute surface curvature using discrete Laplace-Beltrami operator
 * 
 * Computes mean curvature (H), principal curvatures (K1, K2), and curvature tensor
 * for each vertex using a discrete Laplacian with cotangent weights.
 * 
 * Returns a DataFrame containing:
 * - "mean_curvature": Mean curvature (H)
 * - "k1": Maximum principal curvature
 * - "k2": Minimum principal curvature
 * - "gaussian_curvature": Gaussian curvature (K = K1 * K2)
 * - "curvature_tensor": 3x3 shape operator matrix at each vertex
 * - "principal_direction1": Principal direction corresponding to k1
 * - "principal_direction2": Principal direction corresponding to k2
 * 
 * The curvature tensor (shape operator) S maps tangent vectors to tangent vectors:
 * S(v) = -dN(v) where N is the normal field and dN is its derivative.
 * 
 * Principal curvatures are eigenvalues of the shape operator, and
 * principal directions are the corresponding eigenvectors.
 * 
 * Computes discrete mean curvature using:
 * - Cotangent weights for edge contributions
 * - Voronoi area weights for vertices
 * - Vertex normal computation using area-weighted face normals
 * 
 * Computes Gaussian curvature using:
 * - Angle defect method (sum of angles around vertex)
 * - Normalization by vertex areas
 * 
 * Computes principal curvatures (K1, K2) using:
 * - Mean curvature (H) and Gaussian curvature (K)
 * - K1 = H + sqrt(H² - K)
 * - K2 = H - sqrt(H² - K)
 * 
 * Key algorithmic steps:
 * - Builds vertex and edge adjacency information
 * - Computes vertex normals from incident faces
 * - Calculates cotangent weights for the Laplacian
 * - Computes vertex areas using barycentric coordinates
 * - Handles boundary cases and degenerate geometry
 * 
 * @param vertices Vertex positions
 * @param triangles Triangle indices
 * @return DataFrame containing curvature measures
 */
Dataframe surface_curvature(const Positions3 &vertices,
                            const Triangles &triangles);

} // namespace df

#include "inline/curvature.hxx"