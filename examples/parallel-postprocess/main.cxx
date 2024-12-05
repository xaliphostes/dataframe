/*
 * Copyright (c) 2023 fmaerten@gmail.com
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

#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <thread>

#include <dataframe/Serie.h>
#include <dataframe/functional/concat.h>
#include <dataframe/functional/geo/cartesian_grid.h>
#include <dataframe/functional/math/add.h>
#include <dataframe/functional/parallel_execute.h>
#include <dataframe/functional/partition_n.h>
#include <dataframe/functional/print.h>

using Stress = Array;

/**
 * Computes the 3D Green's function (elastic fundamental solution) for a
 * point force in an infinite elastic medium and return the stress tensor.
 *
 * Uses the fundamental solution for an infinite elastic medium
 * - Handles the singularity at r=0
 * - Includes all stress components considering the symmetry of the stress
 *   tensor
 * - Uses efficient computation of common factors
 *
 *The solution is based on the classical elasticity theory and includes:
 *
 * - Distance-dependent terms (1/r³, 1/r⁵)
 * - Angular dependencies through direction cosines
 * - Material property influences through μ and ν
 */
class Source {
  public:
    Source(const Array &pos, const Array force = {1000, 0, 0})
        : fx(force[0]), fy(force[1]), fz(force[2]) {}
    Stress stress(const Array &at) {
        Stress result{0, 0, 0, 0, 0, 0};
        double x = at[0];
        double y = at[1];
        double z = at[2];

        // Compute relative position
        const double dx = x - xs;
        const double dy = y - ys;
        const double dz = z - zs;

        // Compute distance and its powers
        const double r = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (r < 1e-10)
            return result; // Avoid singularity at source point

        const double r3 = r * r * r;
        const double r5 = r3 * r * r;

        // Compute common factors
        const double c1 = mu / (4.0 * M_PI * (1.0 - nu));
        const double c2 = (1.0 - 2.0 * nu);

        // Unit vector components of relative position
        const double nx = dx / r;
        const double ny = dy / r;
        const double nz = dz / r;

        // Compute stress components using Mindlin's solution
        // σxx
        result[0] =
            c1 * (3.0 * (fx * nx * nx + fy * nx * ny + fz * nx * nz) * nx / r3 -
                  (c2 * (fx / r3 - 3.0 * fx * nx * nx / r5)));

        // σxy (xy shear)
        result[1] =
            c1 * (3.0 * (fx * nx * nx + fy * nx * ny + fz * nx * nz) * ny / r3 -
                  (c2 * (fx * ny + fy * nx) / r3));

        // σxz (xz shear)
        result[2] =
            c1 * (3.0 * (fx * nz * nx + fy * nz * ny + fz * nz * nz) * nx / r3 -
                  (c2 * (fx * nz + fz * nx) / r3));

        // σyy
        result[3] =
            c1 * (3.0 * (fx * ny * nx + fy * ny * ny + fz * ny * nz) * ny / r3 -
                  (c2 * (fy / r3 - 3.0 * fy * ny * ny / r5)));

        // σyz (yz shear)
        result[4] =
            c1 * (3.0 * (fx * ny * nx + fy * ny * ny + fz * ny * nz) * nz / r3 -
                  (c2 * (fy * nz + fz * ny) / r3));

        // σzz
        result[5] =
            c1 * (3.0 * (fx * nz * nx + fy * nz * ny + fz * nz * nz) * nz / r3 -
                  (c2 * (fz / r3 - 3.0 * fz * nz * nz / r5)));

        return result;
    }

  private:
    double xs{0}, ys{0}, zs{0};    // Source position
    double fx{1000}, fy{0}, fz{0}; // Source force
    double nu{0.25};               // poisson's ratio
    double mu{1};               // shear modulus
};

/**
 * Definition of the functor (which cumulate the stress due to each source
 * point)
 */
struct Model {
    Model(u_int32_t nbSources = 1e4) {
        df::grid::cartesian::from_points({100, 100}, {-1, -1, -1}, {1, 1, 1})
            .forEach([=](const Array &pos, uint32_t) {
                sources_.push_back(new Source(pos));
            });
    }

    df::Serie operator()(const df::Serie &points) const {
        return points.map([=](const Array &at, uint32_t) {
            Stress s({0, 0, 0, 0, 0, 0});
            for (auto source : sources_) {
                auto stress = source->stress(at);
                // Cumul the stresses
                for (auto i = 0; i < 6; ++i) {
                    s[i] += stress[i];
                }
            }
            return s;
        });
    }
    std::vector<Source *> sources_;
};

int main() {
    u_int32_t nbSources = 1e4;
    u_int32_t nbFields = 1e6;
    uint nbCores = 12; // for parallelization

    Model model(nbSources);

    // An observation grid around the sources (the model)
    df::Serie grid = df::grid::cartesian::from_points(
        {100, 100, 100}, {-10, -10, -10}, {10, 10, 10});

    auto strain = df::parallel_execute(model, df::partition_n(nbCores, grid));
}