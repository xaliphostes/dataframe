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

namespace df {
namespace geo {

/**
 * @brief Compute the curl of a vector field
 * For 2D vector fields, returns scalar field (z component)
 * For 3D vector fields, returns 3D vector field
 * @param field Vector field (must have itemSize 2 or 3)
 * @param coordinates Point coordinates (itemSize 2 for 2D, 3 for 3D)
 * @return GenSerie<T> Curl of the vector field
 */
template <typename T>
inline GenSerie<T> curl(const GenSerie<T> &field, const GenSerie<T> &coordinates) {
    // Validate input dimensions
    const uint32_t coord_dim = coordinates.itemSize();
    const uint32_t field_dim = field.itemSize();

    if (coord_dim != 2 && coord_dim != 3) {
        throw std::invalid_argument(
            "Coordinates must have itemSize 2 or 3 (got " +
            std::to_string(coord_dim) + ")");
    }

    if (field_dim != coord_dim) {
        throw std::invalid_argument(
            "Field must have same itemSize as coordinates");
    }

    if (coordinates.count() != field.count()) {
        throw std::invalid_argument(
            "Coordinates must have same count as field");
    }

    const uint32_t count = field.count();
    const bool is_3d = coord_dim == 3;

    // For 2D vector fields, curl is just a scalar field (z component)
    if (!is_3d) {
        GenSerie<T> result(1, count); // Scalar field output

        // Find typical grid spacing
        T dx = std::numeric_limits<T>::max();
        T dy = std::numeric_limits<T>::max();

        for (uint32_t i = 0; i < count; ++i) {
            auto pi = coordinates.array(i);
            for (uint32_t j = i + 1; j < count; ++j) {
                auto pj = coordinates.array(j);
                T dx_ij = std::abs(pj[0] - pi[0]);
                T dy_ij = std::abs(pj[1] - pi[1]);
                if (dx_ij > 0)
                    dx = std::min(dx, dx_ij);
                if (dy_ij > 0)
                    dy = std::min(dy, dy_ij);
            }
        }

        // Compute curl at each point
        for (uint32_t i = 0; i < count; ++i) {
            auto v = field.array(i);
            auto p = coordinates.array(i);
            T dvx_dy = 0;
            T dvy_dx = 0;
            int nx = 0, ny = 0;

            // Find neighboring points and compute derivatives
            for (uint32_t j = 0; j < count; ++j) {
                if (i == j)
                    continue;

                auto pj = coordinates.array(j);
                auto vj = field.array(j);

                T dx_j = pj[0] - p[0];
                T dy_j = pj[1] - p[1];

                // x-derivative neighbors
                if (std::abs(dy_j) < 0.5 * dy) {
                    dvy_dx += (vj[1] - v[1]) / dx_j;
                    nx++;
                }

                // y-derivative neighbors
                if (std::abs(dx_j) < 0.5 * dx) {
                    dvx_dy += (vj[0] - v[0]) / dy_j;
                    ny++;
                }
            }

            // Average the derivatives if we found neighbors
            if (nx > 0)
                dvy_dx /= nx;
            if (ny > 0)
                dvx_dy /= ny;

            // Curl_z = dv_y/dx - dv_x/dy
            result.setValue(i, dvy_dx - dvx_dy);
        }
        return result;
    }
    // 3D vector field curl
    else {
        GenSerie<T> result(3, count); // Vector field output

        // Find typical grid spacing
        T dx = std::numeric_limits<T>::max();
        T dy = std::numeric_limits<T>::max();
        T dz = std::numeric_limits<T>::max();

        for (uint32_t i = 0; i < count; ++i) {
            auto pi = coordinates.array(i);
            for (uint32_t j = i + 1; j < count; ++j) {
                auto pj = coordinates.array(j);
                T dx_ij = std::abs(pj[0] - pi[0]);
                T dy_ij = std::abs(pj[1] - pi[1]);
                T dz_ij = std::abs(pj[2] - pi[2]);
                if (dx_ij > 0)
                    dx = std::min(dx, dx_ij);
                if (dy_ij > 0)
                    dy = std::min(dy, dy_ij);
                if (dz_ij > 0)
                    dz = std::min(dz, dz_ij);
            }
        }

        // Compute curl at each point
        for (uint32_t i = 0; i < count; ++i) {
            auto v = field.array(i);
            auto p = coordinates.array(i);

            std::vector<T> dvx = {0, 0, 0}; // derivatives wrt x
            std::vector<T> dvy = {0, 0, 0}; // derivatives wrt y
            std::vector<T> dvz = {0, 0, 0}; // derivatives wrt z
            int nx = 0, ny = 0, nz = 0;

            // Find neighboring points and compute derivatives
            for (uint32_t j = 0; j < count; ++j) {
                if (i == j)
                    continue;

                auto pj = coordinates.array(j);
                auto vj = field.array(j);

                T dx_j = pj[0] - p[0];
                T dy_j = pj[1] - p[1];
                T dz_j = pj[2] - p[2];

                // x-derivative neighbors
                if (std::abs(dy_j) < 0.5 * dy && std::abs(dz_j) < 0.5 * dz) {
                    for (int k = 0; k < 3; ++k) {
                        dvx[k] += (vj[k] - v[k]) / dx_j;
                    }
                    nx++;
                }

                // y-derivative neighbors
                if (std::abs(dx_j) < 0.5 * dx && std::abs(dz_j) < 0.5 * dz) {
                    for (int k = 0; k < 3; ++k) {
                        dvy[k] += (vj[k] - v[k]) / dy_j;
                    }
                    ny++;
                }

                // z-derivative neighbors
                if (std::abs(dx_j) < 0.5 * dx && std::abs(dy_j) < 0.5 * dy) {
                    for (int k = 0; k < 3; ++k) {
                        dvz[k] += (vj[k] - v[k]) / dz_j;
                    }
                    nz++;
                }
            }

            // Average the derivatives if we found neighbors
            if (nx > 0)
                for (auto &d : dvx)
                    d /= nx;
            if (ny > 0)
                for (auto &d : dvy)
                    d /= ny;
            if (nz > 0)
                for (auto &d : dvz)
                    d /= nz;

            // Curl components:
            // curl_x = dw/dy - dv/dz
            // curl_y = du/dz - dw/dx
            // curl_z = dv/dx - du/dy
            std::vector<T> curl_vec = {
                dvy[2] - dvz[1], // curl_x
                dvz[0] - dvx[2], // curl_y
                dvx[1] - dvy[0]  // curl_z
            };

            result.setArray(i, curl_vec);
        }
        return result;
    }
}

// Helper function to create a curl operation
template <typename T> inline auto make_curl(const GenSerie<T> &coordinates) {
    return [&coordinates](const GenSerie<T> &field) {
        return curl(field, coordinates);
    };
}

} // namespace geo
} // namespace df
