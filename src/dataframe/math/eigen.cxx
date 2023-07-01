#include <dataframe/math/eigen.h>
#include <cmath>

namespace df
{
    void symmetricEigen(const double *, int, double *, double *);

    Serie eigenValues(const Serie &serie)
    {
        if (serie.dimension() == 2 && serie.itemSize() != 3) {
            return Serie();
        }
        if (serie.dimension() == 3 && serie.itemSize() != 6) {
            return Serie();
        }

        if (serie.dimension() == 2 && serie.itemSize() == 3) {
            double eigen_vec[4] ;
            double eigen_val[2] ;
            double m[3] ;
            return serie.map( [&](const Array& mat, uint32_t) {
                m[0] = mat[0];
                m[1] = mat[1];
                m[2] = mat[2];
                symmetricEigen(m, 2, eigen_vec, eigen_val);
                return Array{eigen_val[0], eigen_val[1]};
            });
        }

        double eigen_vec[9] ;
        double eigen_val[3] ;
        double m[6] ;
        return serie.map( [&](const Array& mat, uint32_t) {
            m[0] = mat[0] ;
            m[1] = mat[1] ;
            m[2] = mat[3] ;
            m[3] = mat[2] ;
            m[4] = mat[4] ;
            m[5] = mat[5] ;
            symmetricEigen(m, 3, eigen_vec, eigen_val);
            return Array{eigen_val[0], eigen_val[1], eigen_val[2]};
        });
    }

    Serie eigenVectors(const Serie &serie)
    {
        if (serie.dimension() == 2 && serie.itemSize() != 3) {
            return Serie();
        }
        if (serie.dimension() == 3 && serie.itemSize() != 6) {
            return Serie();
        }

        if (serie.dimension() == 2 && serie.itemSize() == 3) {
            double eigen_vec[4] ;
            double eigen_val[2] ;
            double m[3] ;
            return serie.map( [&](const Array& mat, uint32_t) {
                m[0] = mat[0];
                m[1] = mat[1];
                m[2] = mat[2];
                symmetricEigen(m, 2, eigen_vec, eigen_val);
                return Array{eigen_vec[0], eigen_vec[1], eigen_vec[2], eigen_vec[3]};
            });
        }

        double eigen_vec[9] ;
        double eigen_val[3] ;
        double m[6] ;
        return serie.map( [&](const Array& mat, uint32_t) {
            m[0] = mat[0] ;
            m[1] = mat[1] ;
            m[2] = mat[3] ;
            m[3] = mat[2] ;
            m[4] = mat[4] ;
            m[5] = mat[5] ;
            symmetricEigen(m, 3, eigen_vec, eigen_val);
            return Array{
                eigen_vec[0], eigen_vec[1], eigen_vec[2],
                eigen_vec[3], eigen_vec[4], eigen_vec[5],
                eigen_vec[6], eigen_vec[7], eigen_vec[8]
            };
        });
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
     * \note IMPORANT: Eigen -values and -vectors are ordered from the highest to the lowest
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
    void symmetricEigen(const double *mat, int n, double *eigen_vec, double *eigen_val)
    {
        static const double EPS = 0.00001;
        static int MAX_ITER = 100;

        int nb_iter = 0;
        int nn = (n * (n + 1)) / 2;
        double *a = new double[nn];
        double *v = new double[n * n];
        int *index = new int[n];

        for (int ij = 0; ij < nn; ++ij)
        {
            a[ij] = mat[ij];
        }

        --a;
        int ij = 0;

        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                if (i == j)
                {
                    v[ij++] = 1.0;
                }
                else
                {
                    v[ij++] = 0.0;
                }
            }
        }

        --v;
        ij = 1;
        double a_norm = 0.0;

        for (int i = 1; i <= n; ++i)
        {
            for (int j = 1; j <= i; ++j)
            {
                if (i != j)
                {
                    double a_ij = a[ij];
                    a_norm += a_ij * a_ij;
                }
                ++ij;
            }
        }

        if (a_norm != 0.0)
        {
            double a_normEPS = a_norm * EPS;
            double thr = a_norm;

            while (thr > a_normEPS && nb_iter < MAX_ITER)
            {
                ++nb_iter;
                double thr_nn = thr / nn;

                for (int l = 1; l < n; ++l)
                {
                    for (int m = l + 1; m <= n; ++m)
                    {
                        int lq = (l * l - l) / 2;
                        int mq = (m * m - m) / 2;
                        int lm = l + mq;
                        double a_lm = a[lm];
                        double a_lm_2 = a_lm * a_lm;

                        if (a_lm_2 < thr_nn)
                        {
                            continue;
                        }

                        int ll = l + lq;
                        int mm = m + mq;
                        double a_ll = a[ll];
                        double a_mm = a[mm];
                        double delta = a_ll - a_mm;

                        double x = 0;
                        if (delta == 0.0)
                        {
                            x = -M_PI / 4;
                        }
                        else
                        {
                            x = -std::atan((a_lm + a_lm) / delta) / 2.0;
                        }

                        double sinx = std::sin(x);
                        double cosx = std::cos(x);
                        double sinx_2 = sinx * sinx;
                        double cosx_2 = cosx * cosx;
                        double sincos = sinx * cosx;
                        int ilv = n * (l - 1);
                        int imv = n * (m - 1);

                        for (int i = 1; i <= n; ++i)
                        {
                            if ((i != l) && (i != m))
                            {
                                int iq = (i * i - i) / 2;
                                int im = 0;
                                if (i < m)
                                {
                                    im = i + mq;
                                }
                                else
                                {
                                    im = m + iq;
                                }
                                double a_im = a[im];
                                int il = 0;
                                if (i < l)
                                {
                                    il = i + lq;
                                }
                                else
                                {
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

        for (int i = 0; i < n; ++i)
        {
            int k = i + (i * (i + 1)) / 2;
            eigen_val[i] = a[k];
        }

        delete[] a;

        for (int i = 0; i < n; ++i)
        {
            index[i] = i;
        }

        for (int i = 0; i < (n - 1); ++i)
        {
            double x = eigen_val[i];
            int k = i;

            for (int j = i + 1; j < n; ++j)
            {
                if (x < eigen_val[j])
                {
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
        for (int k = 0; k < n; ++k)
        {
            int ik = index[k] * n;
            for (int i = 0; i < n; ++i)
            {
                eigen_vec[ij++] = v[ik++];
            }
        }

        delete[] v;
        delete[] index;
    }

}
