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

namespace detail {

// Helper to get matrix dimension from storage size
template <size_t N> constexpr size_t get_matrix_dim() {
    if constexpr (N == 3)
        return 2; // 2x2 symmetric
    else if constexpr (N == 6)
        return 3; // 3x3 symmetric
    else if constexpr (N == 10)
        return 4; // 4x4 symmetric
    else
        static_assert(N == 3 || N == 6 || N == 10,
                      "Unsupported matrix dimension");
}

// Convert from row symmetric to column symmetric storage
template <typename T, size_t N>
void row_to_col_symmetric(const T *row_sym, T *col_sym) {
    if constexpr (N == 3) { // 2x2
        // row:    [a11, a12, a22]
        // column: [a11, a21, a22]
        col_sym[0] = row_sym[0];   // a11
        col_sym[1] = row_sym[1];   // a21 (=a12)
        col_sym[2] = row_sym[2];   // a22
    } else if constexpr (N == 6) { // 3x3
        // row:    [a11, a12, a13, a22, a23, a33]
        // column: [a11, a21, a31, a22, a32, a33]
        col_sym[0] = row_sym[0];    // a11
        col_sym[1] = row_sym[1];    // a21
        col_sym[2] = row_sym[2];    // a31
        col_sym[3] = row_sym[3];    // a22
        col_sym[4] = row_sym[4];    // a32
        col_sym[5] = row_sym[5];    // a33
    } else if constexpr (N == 10) { // 4x4
        // row:    [a11, a12, a13, a14, a22, a23, a24, a33, a34, a44]
        // column: [a11, a21, a31, a41, a22, a32, a42, a33, a43, a44]
        col_sym[0] = row_sym[0]; // a11
        col_sym[1] = row_sym[1]; // a21
        col_sym[2] = row_sym[2]; // a31
        col_sym[3] = row_sym[3]; // a41
        col_sym[4] = row_sym[4]; // a22
        col_sym[5] = row_sym[5]; // a32
        col_sym[6] = row_sym[6]; // a42
        col_sym[7] = row_sym[7]; // a33
        col_sym[8] = row_sym[8]; // a43
        col_sym[9] = row_sym[9]; // a44
    }
}

// Convert from column symmetric back to row symmetric storage
template <typename T, size_t N>
void col_to_row_symmetric(const T *col_sym, T *row_sym) {
    if constexpr (N == 3) {         // 2x2
        row_sym[0] = col_sym[0];    // a11
        row_sym[1] = col_sym[1];    // a12 (=a21)
        row_sym[2] = col_sym[2];    // a22
    } else if constexpr (N == 6) {  // 3x3
        row_sym[0] = col_sym[0];    // a11
        row_sym[1] = col_sym[1];    // a12
        row_sym[2] = col_sym[2];    // a13
        row_sym[3] = col_sym[3];    // a22
        row_sym[4] = col_sym[4];    // a23
        row_sym[5] = col_sym[5];    // a33
    } else if constexpr (N == 10) { // 4x4
        row_sym[0] = col_sym[0];    // a11
        row_sym[1] = col_sym[1];    // a12
        row_sym[2] = col_sym[2];    // a13
        row_sym[3] = col_sym[3];    // a14
        row_sym[4] = col_sym[4];    // a22
        row_sym[5] = col_sym[5];    // a23
        row_sym[6] = col_sym[6];    // a24
        row_sym[7] = col_sym[7];    // a33
        row_sym[8] = col_sym[8];    // a34
        row_sym[9] = col_sym[9];    // a44
    }
}

/**
 * \brief Computes the eigen values and eigen vectors
 * of a semi definite symmetric matrix
 * \param  matrix is stored in column symmetric storage, i.e.
 *     matrix = { m11, m12, m22, m13, m23, m33, m14, m24, m34, m44 ... }
 *     size = n(n+1)/2
 * \param eigen_vectors (return) = { v1, v2, v3, ..., vn }
 *   where vk = vk0, vk1, ..., vkn
 *   size = n^2, must be allocated by caller
 * \param eigen_values  (return) are in decreasing order
 *   size = n,   must be allocated by caller
 *
 * \note IMPORANT: Eigen -values and -vectors are ordered from the highest to
 * the lowest
 *
 * @example
 * ```c++
 * double eigen_vec[9] ;
 * double eigen_val[3] ;
 * double m[6] ;
 *  m[0] = ... ;
 * symmetricEigen(m, 3, eigen_vec, eigen_val) ;
 * ```
 */
void symmetricEigen(const double *mat, int n, double *eigen_vec,
                    double *eigen_val) {
    static const double EPS = 0.00001;
    static int MAX_ITER = 100;

    int nb_iter = 0;
    int nn = (n * (n + 1)) / 2;
    double *a = new double[nn];
    double *v = new double[n * n];
    int *index = new int[n];

    for (int ij = 0; ij < nn; ++ij) {
        a[ij] = mat[ij];
    }

    --a;
    int ij = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                v[ij++] = 1.0;
            } else {
                v[ij++] = 0.0;
            }
        }
    }

    --v;
    ij = 1;
    double a_norm = 0.0;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= i; ++j) {
            if (i != j) {
                double a_ij = a[ij];
                a_norm += a_ij * a_ij;
            }
            ++ij;
        }
    }

    if (a_norm != 0.0) {
        double a_normEPS = a_norm * EPS;
        double thr = a_norm;

        while (thr > a_normEPS && nb_iter < MAX_ITER) {
            ++nb_iter;
            double thr_nn = thr / nn;

            for (int l = 1; l < n; ++l) {
                for (int m = l + 1; m <= n; ++m) {
                    int lq = (l * l - l) / 2;
                    int mq = (m * m - m) / 2;
                    int lm = l + mq;
                    double a_lm = a[lm];
                    double a_lm_2 = a_lm * a_lm;

                    if (a_lm_2 < thr_nn) {
                        continue;
                    }

                    int ll = l + lq;
                    int mm = m + mq;
                    double a_ll = a[ll];
                    double a_mm = a[mm];
                    double delta = a_ll - a_mm;

                    double x = 0;
                    if (delta == 0.0) {
                        x = -M_PI / 4;
                    } else {
                        x = -std::atan((a_lm + a_lm) / delta) / 2.0;
                    }

                    double sinx = std::sin(x);
                    double cosx = std::cos(x);
                    double sinx_2 = sinx * sinx;
                    double cosx_2 = cosx * cosx;
                    double sincos = sinx * cosx;
                    int ilv = n * (l - 1);
                    int imv = n * (m - 1);

                    for (int i = 1; i <= n; ++i) {
                        if ((i != l) && (i != m)) {
                            int iq = (i * i - i) / 2;
                            int im = 0;
                            if (i < m) {
                                im = i + mq;
                            } else {
                                im = m + iq;
                            }
                            double a_im = a[im];
                            int il = 0;
                            if (i < l) {
                                il = i + lq;
                            } else {
                                il = l + iq;
                            }
                            double a_il = a[il];
                            a[il] = a_il * cosx - a_im * sinx;
                            a[im] = a_il * sinx + a_im * cosx;
                        }

                        ++ilv;
                        ++imv;
                        double v_ilv = v[ilv];
                        double v_imv = v[imv];
                        v[ilv] = cosx * v_ilv - sinx * v_imv;
                        v[imv] = sinx * v_ilv + cosx * v_imv;
                    }
                    x = a_lm * sincos;
                    x += x;
                    a[ll] = a_ll * cosx_2 + a_mm * sinx_2 - x;
                    a[mm] = a_ll * sinx_2 + a_mm * cosx_2 + x;
                    a[lm] = 0.0;
                    thr = std::fabs(thr - a_lm_2);
                }
            }
        }
    }

    ++a;

    for (int i = 0; i < n; ++i) {
        int k = i + (i * (i + 1)) / 2;
        eigen_val[i] = a[k];
    }

    delete[] a;

    for (int i = 0; i < n; ++i) {
        index[i] = i;
    }

    for (int i = 0; i < (n - 1); ++i) {
        double x = eigen_val[i];
        int k = i;

        for (int j = i + 1; j < n; ++j) {
            if (x < eigen_val[j]) {
                k = j;
                x = eigen_val[j];
            }
        }

        eigen_val[k] = eigen_val[i];
        eigen_val[i] = x;

        int jj = index[k];
        index[k] = index[i];
        index[i] = jj;
    }

    ++v;

    ij = 0;
    for (int k = 0; k < n; ++k) {
        int ik = index[k] * n;
        for (int i = 0; i < n; ++i) {
            eigen_vec[ij++] = v[ik++];
        }
    }

    delete[] v;
    delete[] index;
}

} // namespace detail

template <typename T, size_t N>
inline Serie<std::array<T, N>> eigenVectors(const Serie<std::array<T, N>> &serie) {
    static_assert(std::is_arithmetic<T>::value,
                  "eigenVectors requires arithmetic type");

    constexpr size_t dim = detail::get_matrix_dim<N>();
    // MSG(N);
    // MSG(dim);

    return serie.map([](const auto &mat, size_t) {
        std::array<T, N> col_mat;
        std::array<T, N> col_vectors;
        std::array<T, dim> values;
        std::array<T, N> row_vectors;

        // Convert to column storage for symmetricEigen
        detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

        // Compute eigenvectors
        detail::symmetricEigen(col_mat.data(), dim, col_vectors.data(),
                               values.data());

        // Convert eigenvectors back to row storage
        // detail::col_to_row_symmetric<T, N>(col_vectors.data(),
        //                                    row_vectors.data());

        return row_vectors;
    });
}

template <typename T, size_t N>
inline Serie<std::array<T, detail::get_matrix_dim<N>()>>
eigenValues(const Serie<std::array<T, N>> &serie) {
    static_assert(std::is_arithmetic<T>::value,
                  "eigenValues requires arithmetic type");

    constexpr size_t dim = detail::get_matrix_dim<N>();

    return serie.map([](const auto &mat, size_t) {
        std::array<T, N> col_mat;
        std::array<T, N> vectors;
        std::array<T, dim> values;

        // Convert to column storage for symmetricEigen
        detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

        // Compute eigenvalues
        detail::symmetricEigen(col_mat.data(), dim, vectors.data(),
                               values.data());

        return values;
    });
}

template <typename T, size_t N>
inline std::pair<Serie<std::array<T, N>>,
          Serie<std::array<T, detail::get_matrix_dim<N>()>>>
eigenSystem(const Serie<std::array<T, N>> &serie) {
    static_assert(std::is_arithmetic<T>::value,
                  "eigenSystem requires arithmetic type");

    constexpr size_t dim = detail::get_matrix_dim<N>();

    std::vector<std::array<T, N>> vectors;
    std::vector<std::array<T, dim>> values;
    vectors.reserve(serie.size());
    values.reserve(serie.size());

    serie.forEach([&](const auto &mat, size_t) {
        std::array<T, N> col_mat;
        std::array<T, N> col_vectors;
        std::array<T, dim> eigenvalues;
        std::array<T, N> row_vectors;

        // Convert to column storage for symmetricEigen
        detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

        // Compute eigensystem
        detail::symmetricEigen(col_mat.data(), dim, col_vectors.data(),
                               eigenvalues.data());

        // Convert eigenvectors back to row storage
        // detail::col_to_row_symmetric<T, N>(col_vectors.data(),
        //                                    row_vectors.data());

        vectors.push_back(row_vectors);
        values.push_back(eigenvalues);
    });

    return {Serie<std::array<T, N>>(vectors),
            Serie<std::array<T, dim>>(values)};
}

// Binding functions for pipeline operations
template <typename T, size_t N> inline auto bind_eigenVectors() {
    return [](const Serie<std::array<T, N>> &serie) {
        return eigenVectors(serie);
    };
}

template <typename T, size_t N> inline auto bind_eigenValues() {
    return
        [](const Serie<std::array<T, N>> &serie) { return eigenValues(serie); };
}

template <typename T, size_t N> inline auto bind_eigenSystem() {
    return
        [](const Serie<std::array<T, N>> &serie) { return eigenSystem(serie); };
}

} // namespace df