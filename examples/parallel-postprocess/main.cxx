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
#include <dataframe/functional/geo/cartesian_grid.h>
#include <dataframe/functional/math/add.h>
#include <dataframe/functional/utils/concat.h>
#include <dataframe/functional/utils/parallel_execute.h>
#include <dataframe/functional/utils/partition_n.h>
#include <dataframe/functional/utils/print.h>

using Stress = Array;

/**
 * Computes the 3D Green's function (elastic fundamental solution) using the
 * fundamental solution for a displacement discontinuity (or dislocation) rather
 * than a point force.
 * Implementation Approach:
 * - First computes displacement gradients using the fundamental solution
 * - Then uses Hooke's law to compute the resulting stresses
 * - Includes both Lamé parameters (μ and λ) for the full elastic response
 *
 * Mathematical Details:
 * - Uses a different set of fundamental solutions based on displacement
 *   discontinuities
 * - Includes terms up to r⁻⁵ for the displacement gradients
 * - Properly handles both deviatoric and volumetric deformation through Lamé
 *   parameters
 *
 * This implementation is particularly useful for:
 * - Dislocation problems
 * - Crack problems
 * - Problems involving prescribed boundary displacements
 * - Material inclusion problems
 */
class Source {
  public:
    Source(const Array &pos, const Array &U = {1, 0, 0}) : pos_(pos), U_(U) {}
    Stress stress(const Array &at) const {
        double x = at[0];
        double y = at[1];
        double z = at[2];

        // Compute relative position
        const double dx = x - pos_[0];
        const double dy = y - pos_[1];
        const double dz = z - pos_[2];
        const double r = std::sqrt(dx * dx + dy * dy + dz * dz);

        if (r < 1e-10) {
            // Avoid singularity at source point
            return Stress{0, 0, 0, 0, 0, 0};
        }

        const double r2 = r * r;
        const double r3 = r2 * r;
        const double r4 = r2 * r2;
        const double r5 = r3 * r2;

        // Compute common factors
        double a = 2.0 * mu;
        // Lamé's first parameter
        const double lambda = a * nu / (1.0 - 2.0 * nu);
        const double c1 = mu / (4.0 * M_PI * (1.0 - nu));
        const double c2 = 3.0 - 4.0 * nu;

        // Unit vector components of relative position
        const double nx = dx / r;
        const double ny = dy / r;
        const double nz = dz / r;

        // Auxiliary terms for displacement gradient
        const double D1 = (c2 / r3) - (3.0 / r5) * (dx * dx);
        const double D2 = (c2 / r3) - (3.0 / r5) * (dy * dy);
        const double D3 = (c2 / r3) - (3.0 / r5) * (dz * dz);
        const double D4 = -(3.0 / r5) * (dx * dy);
        const double D5 = -(3.0 / r5) * (dy * dz);
        const double D6 = -(3.0 / r5) * (dx * dz);

        const double ux = U_[0];
        const double uy = U_[1];
        const double uz = U_[2];

        // First compute displacement gradients
        const double dux_dx = ux * D1 + uy * D4 + uz * D6;
        const double dux_dy = ux * D4 + uy * (-1.0 / r3) + uz * 0.0;
        const double dux_dz = ux * D6 + uy * 0.0 + uz * (-1.0 / r3);

        const double duy_dx = uy * D4 + ux * (-1.0 / r3) + uz * 0.0;
        const double duy_dy = uy * D2 + ux * D4 + uz * D5;
        const double duy_dz = uy * D5 + ux * 0.0 + uz * (-1.0 / r3);

        const double duz_dx = uz * D6 + ux * (-1.0 / r3) + uy * 0.0;
        const double duz_dy = uz * D5 + ux * 0.0 + uy * (-1.0 / r3);
        const double duz_dz = uz * D3 + ux * D6 + uy * D5;

        // Then compute stresses using Hooke's law

        double b = lambda * (dux_dx + duy_dy + duz_dz);
        Stress stress(6);
        stress[0] = a * dux_dx + b;         // σxx
        stress[1] = mu * (dux_dy + duy_dx); // σxy
        stress[2] = mu * (dux_dz + duz_dx); // σxz
        stress[3] = a * duy_dy + b;         // σyy
        stress[4] = mu * (duy_dz + duz_dy); // σyz
        stress[5] = a * duz_dz + b;         // σzz

        return stress;
    }

  private:
    Array pos_{0, 0, 0}; // Source position
    Array U_{1,0,0};     // displ. discon.
    double nu{0.25};     // poisson's ratio
    double mu{1};        // shear modulus
};

/**
 * Definition of the functor (which cumulate the stress due to each source)
 */
struct Model {
    Model(u_int32_t nbSources = 1e4) {
        df::grid::cartesian::from_points({100, 100}, {-1, -1, -1}, {1, 1, 1})
            .forEach([=](const Array &pos, uint32_t) {
                sources_.push_back(Source(pos));
            });
    }

    df::Serie operator()(const df::Serie &points) const {
        return points.map([=](const Array &at, uint32_t) {
            Stress s({0, 0, 0, 0, 0, 0});
            for (const auto &source : sources_) {
                auto stress = source.stress(at);
                for (auto i = 0; i < 6; ++i) {
                    s[i] += stress[i]; // cumul the stresses
                }
            }
            return s;
        });
    }
    std::vector<Source> sources_;
};

int main() {
    Model model(10000);

    // An observation grid around the sources (the model)
    df::Serie grid = df::grid::cartesian::from_points(
        {100, 100, 100}, {-10, -10, -10}, {10, 10, 10});

    uint nbCores = 12;
    auto stress = df::utils::parallel_execute(model, grid, nbCores);
}
