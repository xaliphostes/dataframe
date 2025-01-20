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


template <typename T>
inline GenSerie<T> gradient(const GenSerie<T> &field,
                     const std::vector<uint32_t> &grid_size,
                     const std::vector<T> &dx) {
    if (!field.isValid() || field.itemSize() != 1) {
        throw std::invalid_argument(
            "Field must be a scalar field (itemSize=1)");
    }
    if (grid_size.size() != 3) {
        throw std::invalid_argument("Grid size must have 3 components");
    }
    if (dx.size() != 3) {
        throw std::invalid_argument("Spacing must have 3 components");
    }

    const uint32_t nx = grid_size[0];
    const uint32_t ny = grid_size[1];
    const uint32_t nz = grid_size[2];
    const uint32_t total_size = nx * ny * nz;

    if (field.count() != total_size) {
        throw std::invalid_argument(
            "Field size " + std::to_string(field.count()) +
            " doesn't match grid dimensions " + std::to_string(total_size));
    }

    GenSerie<T> result(3, total_size);

    // Helper to get field index from i,j,k coordinates
    auto idx = [nx, ny](uint32_t i, uint32_t j, uint32_t k) -> uint32_t {
        return i + j * nx + k * nx * ny;
    };

    // Compute gradients
    for (uint32_t k = 0; k < nz; ++k) {
        for (uint32_t j = 0; j < ny; ++j) {
            for (uint32_t i = 0; i < nx; ++i) {
                std::vector<T> grad(3, 0);
                uint32_t current = idx(i, j, k);

                // x derivative
                if (nx > 1) {
                    if (i == 0) {
                        grad[0] =
                            (field.value(idx(1, j, k)) - field.value(current)) /
                            dx[0];
                    } else if (i == nx - 1) {
                        grad[0] = (field.value(current) -
                                   field.value(idx(nx - 2, j, k))) /
                                  dx[0];
                    } else {
                        grad[0] = (field.value(idx(i + 1, j, k)) -
                                   field.value(idx(i - 1, j, k))) /
                                  (2 * dx[0]);
                    }
                }

                // y derivative
                if (ny > 1) {
                    if (j == 0) {
                        grad[1] =
                            (field.value(idx(i, 1, k)) - field.value(current)) /
                            dx[1];
                    } else if (j == ny - 1) {
                        grad[1] = (field.value(current) -
                                   field.value(idx(i, ny - 2, k))) /
                                  dx[1];
                    } else {
                        grad[1] = (field.value(idx(i, j + 1, k)) -
                                   field.value(idx(i, j - 1, k))) /
                                  (2 * dx[1]);
                    }
                }

                // z derivative
                if (nz > 1) {
                    if (k == 0) {
                        grad[2] =
                            (field.value(idx(i, j, 1)) - field.value(current)) /
                            dx[2];
                    } else if (k == nz - 1) {
                        grad[2] = (field.value(current) -
                                   field.value(idx(i, j, nz - 2))) /
                                  dx[2];
                    } else {
                        grad[2] = (field.value(idx(i, j, k + 1)) -
                                   field.value(idx(i, j, k - 1))) /
                                  (2 * dx[2]);
                    }
                }

                result.setArray(current, grad);
            }
        }
    }

    return result;
}

template <typename T>
inline auto make_gradient(const std::vector<uint32_t> &grid_size,
                   const std::vector<T> &dx) {
    return [=](const GenSerie<T> &field) {
        return gradient(field, grid_size, dx);
    };
}

} // namespace geo
} // namespace df