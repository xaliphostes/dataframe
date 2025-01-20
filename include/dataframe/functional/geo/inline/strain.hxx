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
 * @brief Compute strain tensor from displacement field
 * @param u Displacement field (itemSize=3)
 * @param grid_size Grid dimensions {nx,ny,nz}
 * @param dx Grid spacing {dx,dy,dz}
 * @return GenSerie<T> with itemSize=6 containing strain tensor components
 *         [εxx, εyy, εzz, εxy, εyz, εxz]
 */
template <typename T>
inline GenSerie<T> strain(const GenSerie<T> &u, const std::vector<uint32_t> &grid_size,
                   const std::vector<T> &dx) {
    if (!u.isValid() || u.itemSize() != 3) {
        throw std::invalid_argument("Displacement field must have itemSize=3");
    }

    // Extract each component of displacement using a vector version of map
    auto extract_component = [](uint32_t comp) {
        return [comp](const std::vector<T> &v, uint32_t) { return v[comp]; };
    };

    // Get scalar fields for each displacement component
    auto grad_ux = gradient(df::map(extract_component(0), u), // x component
                            grid_size, dx);
    auto grad_uy = gradient(df::map(extract_component(1), u), // y component
                            grid_size, dx);
    auto grad_uz = gradient(df::map(extract_component(2), u), // z component
                            grid_size, dx);

    // Combine gradients into strain tensor components
    GenSerie<T> result(6, u.count());
    for (uint32_t i = 0; i < u.count(); ++i) {
        auto gx = grad_ux.array(i);
        auto gy = grad_uy.array(i);
        auto gz = grad_uz.array(i);

        std::vector<T> strain(6);
        // Normal strains
        strain[0] = gx[0]; // εxx = ∂ux/∂x
        strain[1] = gy[1]; // εyy = ∂uy/∂y
        strain[2] = gz[2]; // εzz = ∂uz/∂z
        // Shear strains (symmetric part)
        strain[3] = 0.5 * (gx[1] + gy[0]); // εxy = 1/2(∂ux/∂y + ∂uy/∂x)
        strain[4] = 0.5 * (gy[2] + gz[1]); // εyz = 1/2(∂uy/∂z + ∂uz/∂y)
        strain[5] = 0.5 * (gx[2] + gz[0]); // εxz = 1/2(∂ux/∂z + ∂uz/∂x)

        result.setArray(i, strain);
    }

    return result;
}

/**
 * @brief Create strain operation that can be used with the pipe operator
 */
template <typename T>
inline auto make_strain(const std::vector<uint32_t> &grid_size,
                 const std::vector<T> &dx) {
    return [=](const GenSerie<T> &u) { return strain(u, grid_size, dx); };
}

} // namespace geo
} // namespace df