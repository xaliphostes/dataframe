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
namespace algebra {

// ----------------------------------------------

namespace details {

template <typename F, typename T> class EigenOp {
  public:
    explicit EigenOp(F func) : func_(std::move(func)) {}
    auto operator()(const GenSerie<T> &serie) const { return func_(serie); }

  private:
    F func_;
};

template <typename T> void symmetricEigen(const T *, int, T *, T *);

} // namespace details

// ----------------------------------------------

template <typename T> inline GenSerie<T> eigenValues(const GenSerie<T> &serie) {
    if (serie.itemSize() == 3) { // 2D symmetric
        return df::map(
            [](const Array<T> &mat, uint32_t) {
                T eigen_vec[4];
                T eigen_val[2];
                details::symmetricEigen(mat.data(), 2, eigen_vec, eigen_val);
                return Array<T>{eigen_val[0], eigen_val[1]};
            },
            serie);
    } else if (serie.itemSize() == 6) { // 3D symmetric
        return df::map(
            [](const Array<T> &mat, uint32_t) {
                T eigen_vec[9];
                T eigen_val[3];
                T m[6] = {mat[0], mat[1], mat[3], mat[2], mat[4], mat[5]};
                details::symmetricEigen(m, 3, eigen_vec, eigen_val);
                return Array<T>{eigen_val[0], eigen_val[1], eigen_val[2]};
            },
            serie);
    }
    throw std::runtime_error("Unsupported matrix size");
};

template <typename T>
inline GenSerie<T> eigenVectors(const GenSerie<T> &serie) {
    if (serie.itemSize() == 3) { // 2D symmetric
        return df::map(
            [](const Array<T> &mat, uint32_t) {
                T eigen_vec[4];
                T eigen_val[2];
                details::symmetricEigen(mat.data(), 2, eigen_vec, eigen_val);
                return Array<T>{eigen_vec[0], eigen_vec[1], eigen_vec[2],
                                eigen_vec[3]};
            },
            serie);
    } else if (serie.itemSize() == 6) { // 3D symmetric
        return df::map(
            [](const Array<T> &mat, uint32_t) {
                T eigen_vec[9];
                T eigen_val[3];
                T m[6] = {mat[0], mat[1], mat[3], mat[2], mat[4], mat[5]};
                details::symmetricEigen(m, 3, eigen_vec, eigen_val);
                return Array<T>{eigen_vec[0], eigen_vec[1], eigen_vec[2],
                                eigen_vec[3], eigen_vec[4], eigen_vec[5],
                                eigen_vec[6], eigen_vec[7], eigen_vec[8]};
            },
            serie);
    }
    throw std::runtime_error("Unsupported matrix size");
};

template <typename T>
inline EigenSystem<T> eigenSystem(const GenSerie<T> &serie) {
    if (serie.itemSize() == 3) { // 2D symmetric
        auto values = GenSerie<T>(2, serie.count());
        auto vectors = GenSerie<T>(4, serie.count());

        for (uint32_t i = 0; i < serie.count(); ++i) {
            T eigen_vec[4];
            T eigen_val[2];
            const auto &mat = serie.array(i);
            details::symmetricEigen(mat.data(), 2, eigen_vec, eigen_val);
            values.set(i, Array<T>{eigen_val[0], eigen_val[1]});
            vectors.set(i, Array<T>{eigen_vec[0], eigen_vec[1], eigen_vec[2],
                                    eigen_vec[3]});
        }
        return {values, vectors};
    } else if (serie.itemSize() == 6) { // 3D symmetric
        auto values = GenSerie<T>(3, serie.count());
        auto vectors = GenSerie<T>(9, serie.count());

        for (uint32_t i = 0; i < serie.count(); ++i) {
            T eigen_vec[9];
            T eigen_val[3];
            const auto &mat = serie.array(i);
            T m[6] = {mat[0], mat[1], mat[3], mat[2], mat[4], mat[5]};
            details::symmetricEigen(m, 3, eigen_vec, eigen_val);
            values.set(i, Array<T>{eigen_val[0], eigen_val[1], eigen_val[2]});
            vectors.set(i, Array<T>{eigen_vec[0], eigen_vec[1], eigen_vec[2],
                                    eigen_vec[3], eigen_vec[4], eigen_vec[5],
                                    eigen_vec[6], eigen_vec[7], eigen_vec[8]});
        }
        return {values, vectors};
    }
    throw std::runtime_error("Unsupported matrix size");
};

template <typename T> inline auto make_eigenValues() {
    return details::EigenOp<GenSerie<T>(const GenSerie<T> &), T>(
        eigenValues<T>);
}

template <typename T> inline auto make_eigenVectors() {
    return details::EigenOp<GenSerie<T>(const GenSerie<T> &), T>(
        eigenVectors<T>);
}

template <typename T> inline auto make_eigenSystem() {
    return details::EigenOp<GenSerie<T>(const GenSerie<T> &), T>(
        eigenSystem<T>);
}

// ---------------------- PRIVATE ------------------------

namespace details {

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
 * T eigen_vec[9] ;
 * T eigen_val[3] ;
 * T m[6] ;
 *  m[0] = ... ;
 * symmetricEigen(m, 3, eigen_vec, eigen_val) ;
 * ```
 */
template <typename T>
void symmetricEigen(const T *mat, int n, T *eigen_vec, T *eigen_val) {
    static const T EPS = 0.00001;
    static int MAX_ITER = 100;

    int nb_iter = 0;
    int nn = (n * (n + 1)) / 2;
    T *a = new T[nn];
    T *v = new T[n * n];
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
    T a_norm = 0.0;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= i; ++j) {
            if (i != j) {
                T a_ij = a[ij];
                a_norm += a_ij * a_ij;
            }
            ++ij;
        }
    }

    if (a_norm != 0.0) {
        T a_normEPS = a_norm * EPS;
        T thr = a_norm;

        while (thr > a_normEPS && nb_iter < MAX_ITER) {
            ++nb_iter;
            T thr_nn = thr / nn;

            for (int l = 1; l < n; ++l) {
                for (int m = l + 1; m <= n; ++m) {
                    int lq = (l * l - l) / 2;
                    int mq = (m * m - m) / 2;
                    int lm = l + mq;
                    T a_lm = a[lm];
                    T a_lm_2 = a_lm * a_lm;

                    if (a_lm_2 < thr_nn) {
                        continue;
                    }

                    int ll = l + lq;
                    int mm = m + mq;
                    T a_ll = a[ll];
                    T a_mm = a[mm];
                    T delta = a_ll - a_mm;

                    T x = 0;
                    if (delta == 0.0) {
                        x = -M_PI / 4;
                    } else {
                        x = -std::atan((a_lm + a_lm) / delta) / 2.0;
                    }

                    T sinx = std::sin(x);
                    T cosx = std::cos(x);
                    T sinx_2 = sinx * sinx;
                    T cosx_2 = cosx * cosx;
                    T sincos = sinx * cosx;
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
                            T a_im = a[im];
                            int il = 0;
                            if (i < l) {
                                il = i + lq;
                            } else {
                                il = l + iq;
                            }
                            T a_il = a[il];
                            a[il] = a_il * cosx - a_im * sinx;
                            a[im] = a_il * sinx + a_im * cosx;
                        }

                        ++ilv;
                        ++imv;
                        T v_ilv = v[ilv];
                        T v_imv = v[imv];
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
        T x = eigen_val[i];
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

} // namespace details
} // namespace algebra
} // namespace df
