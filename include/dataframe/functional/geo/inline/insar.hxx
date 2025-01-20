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

#include <cmath>
#include <dataframe/functional/algebra/dot.h>
#include <dataframe/functional/map.h>

namespace df {
namespace geo {

/**
 * @brief Compute the insar Serie (itemSize = 1) from a displacement vector
 * field u
 * @param u The displacement vector field (itemSize = 3)
 * @param los The satellite direction (a 3D vector)
 * @return GenSerie<T> of itemSize=1 containing dot products
 */
template <typename T>
GenSerie<T> insar(const GenSerie<T> &u, const std::vector<T> &los) {
    if (!u.isValid() || u.itemSize() != 3) {
        return GenSerie<T>();
    }

    if (los.size() != 3) {
        return GenSerie<T>();
    }

    GenSerie<T> result(1, u.count());
    for (uint32_t i = 0; i < u.count(); ++i) {
        auto disp = u.array(i);
        result.setValue(i,
                        disp[0] * los[0] + disp[1] * los[1] + disp[2] * los[2]);
    }
    return result;
}
// template <typename T>
// GenSerie<T> insar(const GenSerie<T> &u, const std::vector<T> &los) {
//     if (!u.isValid() || u.itemSize() != 3) {
//         return GenSerie<T>();
//     }

//     if (los.size() != 3) {
//         return GenSerie<T>();
//     }

//     // Compute dot product for each displacement vector with LOS
//     return df::map(
//         [&los](const std::vector<T> &displacement, uint32_t) -> T {
//             return displacement[0] * los[0] + displacement[1] * los[1] +
//                    displacement[2] * los[2];
//         },
//         u);
// }

/**
 * @brief Create insar operation that can be used with the pipe operator
 */
template <typename T> auto make_insar(const std::vector<T> &los) {
    return [&los](const GenSerie<T> &u) { return insar(u, los); };
}

/**
 * @brief Helper function to compute fractional part
 */
template <typename T> T frac(T value) { return value - std::floor(value); }

/**
 * @brief Compute the fringes given the insar Serie
 * @param insar_data The insar computed from insar function
 * @param fringe_spacing The spacing of the fringes
 * @return GenSerie<T> of itemSize=1
 */
template <typename T>
GenSerie<T> fringes(const GenSerie<T> &insar_data, T fringe_spacing) {
    if (!insar_data.isValid() || insar_data.itemSize() != 1) {
        throw std::runtime_error("Invalid input serie. itemSize must be 1");
    }

    // Convert each value to a fringe value
    return df::map(
        [spacing = fringe_spacing](T value, uint32_t) -> T {
            return std::abs(spacing * frac(value / spacing));
        },
        insar_data);
}

/**
 * @brief Create fringes operation that can be used with the pipe operator
 */
template <typename T> auto make_fringes(T fringe_spacing) {
    return [fringe_spacing](const GenSerie<T> &insar_data) {
        return fringes(insar_data, fringe_spacing);
    };
}

} // namespace geo
} // namespace df