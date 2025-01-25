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

namespace df {
namespace geo {

template <typename T> class GenSphereGenerator {
  public:
    using DArray = std::array<T, 3>;
    using CDArray = const std::array<T, 3>;

    static Dataframe generateSphere(int subdivision,
                                           bool shared = true) {
        if (subdivision < 1) {
            throw std::invalid_argument("Subdivision must be > 0");
        }

        std::vector<T> positions;
        std::vector<uint32_t> indices;
        uint32_t idx = 0;

        // Initialize faces from vertices
        std::vector<std::array<std::array<T, 3>, 3>> faces;
        for (const auto &face : face_indices) {
            faces.push_back(
                {vertices[face[0]], vertices[face[1]], vertices[face[2]]});
        }

        auto newP = [&](const std::array<T, 3> &p0, const std::array<T, 3> &p1,
                        const std::array<T, 3> &p2) {
            for (const auto &p : {p0, p1, p2}) {
                positions.push_back(static_cast<T>(p[0]));
                positions.push_back(static_cast<T>(p[1]));
                positions.push_back(static_cast<T>(p[2]));
                indices.push_back(idx++);
            }
        };

        for (const auto &face : faces) {
            const auto &[f0, f1, f2] = face;
            auto f20 = lerp(f0, f1, static_cast<T>(1.0) / subdivision);
            auto f21 = lerp(f0, f2, static_cast<T>(1.0) / subdivision);
            newP(proj(f0), proj(f20), proj(f21));

            for (int i = 1; i < subdivision; ++i) {
                auto f10 = f20;
                f20 = lerp(f0, f1, static_cast<T>(i + 1.0) / subdivision);
                auto f11 = f21;
                f21 = lerp(f0, f2, static_cast<T>(i + 1.0) / subdivision);

                for (int j = 0; j <= i; ++j) {
                    newP(proj(lerp(f10, f11, static_cast<T>(j) / i)),
                         proj(lerp(f20, f21, static_cast<T>(j) / (i + 1))),
                         proj(lerp(f20, f21, static_cast<T>(j + 1) / (i + 1))));
                }

                for (int j = 0; j < i; ++j) {
                    newP(proj(lerp(f10, f11, static_cast<T>(j) / i)),
                         proj(lerp(f20, f21, static_cast<T>(j + 1) / (i + 1))),
                         proj(lerp(f10, f11, static_cast<T>(j + 1) / i)));
                }
            }
        }

        Dataframe dataf;
        dataf.add("positions", GenSerie<T>(3, positions));
        dataf.add("indices", GenSerie<uint32_t>(3, indices));
        
        return dataf;

        // return {GenSerie<T>(3, positions), GenSerie<uint32_t>(1, indices)};
    }

  private:
    static constexpr T R = 3.0;
    static const T PHI;
    static const std::vector<std::array<T, 3>> vertices;
    static const std::vector<std::array<int, 3>> face_indices;

    static DArray proj(CDArray &p) {
        T k = R / std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
        return {k * p[0], k * p[1], k * p[2]};
    }

    static DArray lerp(CDArray &p0, CDArray &p1, T t) {
        return {p0[0] + t * (p1[0] - p0[0]), p0[1] + t * (p1[1] - p0[1]),
                p0[2] + t * (p1[2] - p0[2])};
    }
};

template <typename T>
const T GenSphereGenerator<T>::PHI =
    static_cast<T>((1.0 + std::sqrt(5.0)) / 2.0);

template <typename T>
const std::vector<std::array<int, 3>> GenSphereGenerator<T>::face_indices = {
    {0, 1, 4},  {1, 9, 4},  {4, 9, 5},  {5, 9, 3},  {2, 3, 7},
    {3, 2, 5},  {7, 10, 2}, {0, 8, 10}, {0, 4, 8},  {8, 2, 10},
    {8, 4, 5},  {8, 5, 2},  {1, 0, 6},  {11, 1, 6}, {3, 9, 11},
    {6, 10, 7}, {3, 11, 7}, {11, 6, 7}, {6, 0, 10}, {9, 1, 11}};

template <typename T>
const std::vector<std::array<T, 3>> GenSphereGenerator<T>::vertices = {
    {1, GenSphereGenerator<T>::PHI, 0},  {-1, GenSphereGenerator<T>::PHI, 0},
    {1, -GenSphereGenerator<T>::PHI, 0}, {-1, -GenSphereGenerator<T>::PHI, 0},
    {0, 1, GenSphereGenerator<T>::PHI},  {0, -1, GenSphereGenerator<T>::PHI},
    {0, 1, -GenSphereGenerator<T>::PHI}, {0, -1, -GenSphereGenerator<T>::PHI},
    {GenSphereGenerator<T>::PHI, 0, 1},  {-GenSphereGenerator<T>::PHI, 0, 1},
    {GenSphereGenerator<T>::PHI, 0, -1}, {-GenSphereGenerator<T>::PHI, 0, -1}};

template <typename T>
Dataframe generateSphere(int subdivision, bool shared) {
    return GenSphereGenerator<T>::generateSphere(subdivision, shared);
}

} // namespace geo
} // namespace df
