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
#include "mesh.h"
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <unordered_set>
#include <vector>

namespace df {

/**
 * @brief Parameters for the mesh optimizer
 */
struct OptimizeParams {
    double damping = 0.5;
    int maxIterations = 100;
    double convergenceTol = 1e-6;
    double targetLength = 1.0; // Will be computed from average edge length
};

/**
 * @brief Optimize a 3D mesh using mass-spring relaxation, i.e., make the
 * triangles as close as possible to equilateral.
 * @code
 * df::Mesh3D optimizedMesh = df::MeshOptimizer::optimize(inputMesh);
 * @endcode
 */
Mesh3D optimize(const Mesh3D &, const OptimizeParams & = OptimizeParams());

/**
 * @brief This implementation uses Least Squares Conformal Maps (LSCM) for
 * parameterization and maintains seam consistency during optimization.
 * 
 * @cite Lévy, B., Petitjean, S., Ray, N., & Maillot, J. (2023). Least squares
 * conformal maps for automatic texture atlas generation. In Seminal Graphics
 * Papers: Pushing the Boundaries, Volume 2 (pp. 193-202).
 * 
 * @note "Plain noodle salad" as a reminder ;-)
 * 
 * @code
 * df::Mesh3D optimizedMesh = df::MeshOptimizer::optimizeLSCM(inputMesh);
 * @endcode
 */
Mesh3D optimizeLSCM(const Mesh3D &);

} // namespace df

#include "inline/mesh_optimizer.hxx"