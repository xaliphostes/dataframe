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
#include <dataframe/algebra/norm.h>
#include <dataframe/geo/mesh/mesh.h>
#include <dataframe/math/scale.h>
#include <map>
#include <memory>
#include <vector>

namespace df {

/**
 * @brief Class implementing harmonic diffusion on a triangulated surface using
 * Mesh Uses Laplace equation to diffuse attributes (scalar or vector) with
 * constraints
 * @tparam N Dimension of the mesh (2 or 3)
 */
template <size_t N> class HarmonicDiffusion {
  public:
    /**
     * @brief Constructor for HarmonicDiffusion
     * @param mesh Input mesh of dimension N
     * @param init_value Initial value (can be scalar or vector)
     */
    template <typename T>
    HarmonicDiffusion(const Mesh<N> &mesh, const T &init_value = T());

    /**
     * @brief Constructor for HarmonicDiffusion
     * @param vertices Vertices of the mesh
     * @param triangles Triangles of the mesh
     * @param init_value Initial value (can be scalar or vector)
     */
    template <typename T>
    HarmonicDiffusion(const Serie<Vector3>&, const Serie<iVector3>&, const T &init_value = T());

    /**
     * @brief Set maximum number of iterations
     */
    void setMaxIter(size_t);

    /**
     * @brief Set convergence threshold
     */
    void setEps(double);

    /**
     * @brief Set smoothing parameter
     */
    void setEpsilon(double);

    /**
     * @brief Add constraint at specific position
     */
    template <typename T>
    void addConstraint(const Vector<N> &pos, const T &);

    /**
     * @brief Constrain all border nodes with given value
     */
    template <typename T> void constrainBorders(const T &);

    /**
     * @brief Solve the harmonic diffusion
     * @param name Name for the result Serie in the DataFrame
     * @param record Whether to record intermediate steps
     * @param step_interval Interval between recorded steps
     * @return DataFrame containing the results
     */
    Dataframe solve(const std::string & = "property", bool = false, size_t = 0);

  private:
    Mesh<N> mesh_;
    std::vector<std::vector<double>> values_;
    std::vector<size_t> constrained_nodes_;
    size_t max_iter_;
    double eps_;
    double epsilon_;

    template <typename T> void initializeValues(const T &init_value);
    size_t findClosestNode(const Vector<N> &pos) const;
    bool isConstrained(size_t node_idx) const;
    Serie<double>
    createSerie(const std::vector<std::vector<double>> &data) const;
};

} // namespace df

#include "inline/harmonic_diffusion.hxx"