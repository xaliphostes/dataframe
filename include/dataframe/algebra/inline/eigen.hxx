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

    namespace eig {

        template <size_t N> struct eigen_matrix_dim;
        template <> struct eigen_matrix_dim<3> {
            uint16_t size { 3 };
            uint16_t dim { 2 };
        };
        template <> struct eigen_matrix_dim<6> {
            uint16_t size { 6 };
            uint16_t dim { 3 };
        };
        template <> struct eigen_matrix_dim<10> {
            uint16_t size { 10 };
            uint16_t dim { 4 };
        };

        // Pack helpers (N -> n and expand packed to full M)
        template <size_t N> inline constexpr int dim_from_N()
        {
            static_assert(N == 3 || N == 6 || N == 10, "Only N=3,6,10 supported");
            return (N == 3 ? 2 : (N == 6 ? 3 : 4));
        }

        template <size_t N> struct EigResult {
            static_assert(
                N == 3 || N == 6 || N == 10, "Supported N are 3 (2x2), 6 (3x3), 10 (4x4).");
            static constexpr int dim = (N == 3 ? 2 : (N == 6 ? 3 : 4));
            typename eigen_values_return_type<N>::type values;
            // Column-major eigenvectors: vectors[i + dim*j] = component i of eigenvector j
            typename eigen_vectors_return_type<N>::type vectors;
        };

        // Internal: small fixed-size Jacobi for n<=4
        inline void jacobiSymmetric(
            double M[4][4], int n, double V[4][4], int maxIter = 50, double tol = 1e-12)
        {
            // Initialize V = I
            for (int i = 0; i < n; ++i)
                for (int j = 0; j < n; ++j)
                    V[i][j] = (i == j ? 1.0 : 0.0);

            for (int iter = 0; iter < maxIter; ++iter) {
                // Find largest off-diagonal entry
                int p = 0, q = 1;
                double maxOff = 0.0;
                for (int i = 0; i < n; ++i)
                    for (int j = i + 1; j < n; ++j) {
                        double a = std::fabs(M[i][j]);
                        if (a > maxOff) {
                            maxOff = a;
                            p = i;
                            q = j;
                        }
                    }
                if (maxOff < tol)
                    break;

                // Jacobi rotation (stable tau formulation)
                double app = M[p][p], aqq = M[q][q], apq = M[p][q];
                double tau = (aqq - app) / (2.0 * apq);
                double t = (tau >= 0.0) ? 1.0 / (tau + std::sqrt(1.0 + tau * tau))
                                        : 1.0 / (tau - std::sqrt(1.0 + tau * tau));
                double c = 1.0 / std::sqrt(1.0 + t * t);
                double s = t * c;

                // Update the rows/cols p and q (use old values once per k)
                for (int k = 0; k < n; ++k) {
                    if (k == p || k == q)
                        continue;
                    double mkp = M[k][p];
                    double mkq = M[k][q];
                    double pk = c * mkp - s * mkq;
                    double qk = s * mkp + c * mkq;
                    M[k][p] = M[p][k] = pk;
                    M[k][q] = M[q][k] = qk;
                }

                // Update the 2x2 block
                double app_new = c * c * app - 2.0 * s * c * apq + s * s * aqq;
                double aqq_new = s * s * app + 2.0 * s * c * apq + c * c * aqq;
                M[p][p] = app_new;
                M[q][q] = aqq_new;
                M[p][q] = M[q][p] = 0.0;

                // Accumulate eigenvectors
                for (int k = 0; k < n; ++k) {
                    double vkp = V[k][p];
                    double vkq = V[k][q];
                    V[k][p] = c * vkp - s * vkq;
                    V[k][q] = s * vkp + c * vkq;
                }
            }
        }

        template <size_t N>
        inline void packedToDense(const std::array<double, N>& a, double M[4][4], int& n)
        {
            n = dim_from_N<N>();
            // zero initialize safe area
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    M[i][j] = 0.0;

            if constexpr (N == 3) {
                // [xx, xy, yy]
                M[0][0] = a[0];
                M[0][1] = M[1][0] = a[1];
                M[1][1] = a[2];
            } else if constexpr (N == 6) {
                // [xx, xy, xz, yy, yz, zz]
                M[0][0] = a[0];
                M[0][1] = M[1][0] = a[1];
                M[0][2] = M[2][0] = a[2];
                M[1][1] = a[3];
                M[1][2] = M[2][1] = a[4];
                M[2][2] = a[5];
            } else { // N == 10
                // [xx, xy, xz, xw, yy, yz, yw, zz, zw, ww]
                M[0][0] = a[0];
                M[0][1] = M[1][0] = a[1];
                M[0][2] = M[2][0] = a[2];
                M[0][3] = M[3][0] = a[3];
                M[1][1] = a[4];
                M[1][2] = M[2][1] = a[5];
                M[1][3] = M[3][1] = a[6];
                M[2][2] = a[7];
                M[2][3] = M[3][2] = a[8];
                M[3][3] = a[9];
            }
        }

        template <size_t N>
        inline void sortEigenpairs(std::array<double, dim_from_N<N>()>& evals,
            std::array<double, dim_from_N<N>() * dim_from_N<N>()>& evecs)
        {
            constexpr int n = dim_from_N<N>();
            // indices 0..n-1
            int idx[n];
            for (int i = 0; i < n; ++i)
                idx[i] = i;
            std::sort(
                idx, idx + n, [&](int i, int j) { return evals[i] > evals[j]; }); // descending

            std::array<double, n> evals_sorted;
            std::array<double, n * n> evecs_sorted;

            for (int j = 0; j < n; ++j) {
                int src = idx[j];
                evals_sorted[j] = evals[src];
                for (int i = 0; i < n; ++i) {
                    evecs_sorted[i + n * j] = evecs[i + n * src];
                }
            }
            evals = evals_sorted;
            evecs = evecs_sorted;
        }

        // Public API
        template <size_t N>
        inline EigResult<N> eigenSymmetricPacked(const std::array<double, N>& packed,
            int maxIter = 50, double tol = 1e-12, bool sort_descending = true)
        {
            EigResult<N> out;
            constexpr int n = EigResult<N>::dim;

            // Expand to dense
            double M[4][4];
            int nn;
            packedToDense(packed, M, nn); // nn == n

            // Run Jacobi
            double V[4][4];
            jacobiSymmetric(M, n, V, maxIter, tol);

            // Fill outputs
            for (int i = 0; i < n; ++i)
                out.values[i] = M[i][i];
            for (int j = 0; j < n; ++j)
                for (int i = 0; i < n; ++i)
                    out.vectors[i + n * j] = V[i][j]; // column-major

            if (sort_descending) {
                sortEigenpairs<N>(out.values, out.vectors);
            }
            return out;
        }

        template <typename T, size_t N>
        inline EigResult<N> eigenSymmetricPacked(const std::array<T, N>& packed, int maxIter = 50,
            double tol = 1e-12, bool sort_descending = true)
        {
            std::array<double, N> d {};
            for (size_t i = 0; i < N; ++i)
                d[i] = static_cast<double>(packed[i]);
            return eigenSymmetricPacked<N>(d, maxIter, tol, sort_descending);
        }

        // Assign one column of length D into a vector-like type Vec.
        // Works if Vec is std::array<double,D> OR if Vec is constructible from D doubles.
        template <int D, typename Vec> inline void assign_col(Vec& dst, const double* col)
        {
            if constexpr (std::is_same_v<Vec, std::array<double, D>>) {
                for (int i = 0; i < D; ++i)
                    dst[i] = col[i];
            } else {
                if constexpr (D == 2)
                    dst = Vec { col[0], col[1] };
                else if constexpr (D == 3)
                    dst = Vec { col[0], col[1], col[2] };
                else /* D==4 */
                    dst = Vec { col[0], col[1], col[2], col[3] };
            }
        }

        // Convert column-major raw (double) eigenvectors into your output container type.
        template <size_t N, typename OutArray /* = typename eigen_vectors_return_type<N>::type */>
        inline OutArray to_vec_columns(
            const std::array<double, dim_from_N<N>() * dim_from_N<N>()>& raw)
        {
            constexpr int D = dim_from_N<N>();
            OutArray out {};
            for (int j = 0; j < D; ++j) {
                auto& vj = out[j]; // VectorD
                assign_col<D>(vj, &raw[D * j]); // raw[i + D*j] is component i of column j
            }
            return out;
        }

    } // namespace eig

    template <typename T, size_t N>
    Serie<typename eigen_vectors_return_type<N>::type> eigenVectors(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenVectors requires arithmetic type");
        static_assert(
            N == 3 || N == 6 || N == 10, "eigenVectors supports only N=3,6,10 for sym matrices");

        Serie<typename eigen_vectors_return_type<N>::type> vectors(serie.size());

        serie.forEach([&vectors](const std::array<T, N>& mat, size_t index) {
            auto result = eig::eigenSymmetricPacked<N>(mat);
            vectors.set(index, result.vectors);
        });

        return vectors;
    }

    template <typename T, size_t N>
    inline Serie<typename eigen_values_return_type<N>::type> eigenValues(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");
        static_assert(
            N == 3 || N == 6 || N == 10, "eigenValues supports only N=3,6,10 for sym matrices");

        Serie<typename eigen_values_return_type<N>::type> values(serie.size());

        serie.forEach([&values](const auto& mat, size_t index) {
            auto result = eig::eigenSymmetricPacked(mat);
            values.set(index, result.values);
        });

        return values;
    }

    template <typename T, size_t N>
    inline std::pair<Serie<typename eigen_values_return_type<N>::type>,
        Serie<typename eigen_vectors_return_type<N>::type>>
    eigenSystem(const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenSystem requires arithmetic type");
        static_assert(
            N == 3 || N == 6 || N == 10, "eigenSystem supports only N=3,6,10 for sym matrices");

        // constexpr size_t dim = eig::eigen_matrix_dim<N>::dim; // eig::dim_from_N<N>();

        Serie<typename eigen_values_return_type<N>::type> values(serie.size());
        Serie<typename eigen_vectors_return_type<N>::type> vectors(serie.size());

        serie.forEach([&values, &vectors](const auto& mat, size_t index) {
            auto result = eig::eigenSymmetricPacked(mat);
            vectors.set(index, result.vectors);
            values.set(index, result.values);
        });

        return std::make_pair(values, vectors);
    }

    /*
    template <typename T, size_t N>
    Serie<typename eigen_vectors_return_type<N>::type> eigenVectors(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenVectors requires arithmetic type");

        constexpr size_t dim = detail::get_matrix_dim<N>();
        using return_type = typename eigen_vectors_return_type<N>::type;

        return serie.map([](const auto& mat, size_t) {
            std::array<T, N> col_mat;
            std::array<T, dim * dim> col_vectors;
            std::array<T, dim> values;
            return_type eigenvectors;

            // Convert to column storage for symmetricEigen
            detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

            // Compute eigenvectors
            detail::symmetricEigen(col_mat.data(), dim, col_vectors.data(), values.data());

            // Convert column storage eigenvectors to array of vectors
            detail::col_vectors_to_array<T, N>(
                col_vectors.data(), reinterpret_cast<T*>(eigenvectors.data()));

            return eigenvectors;
        });
    }

    template <typename T, size_t N>
    inline Serie<std::array<T, detail::get_matrix_dim<N>()>> eigenValues(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");

        constexpr size_t dim = detail::get_matrix_dim<N>();

        return serie.map([](const auto& mat, size_t) {
            std::array<T, N> col_mat;
            std::array<T, N> vectors;
            std::array<T, dim> values;

            // Convert to column storage for symmetricEigen
            detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

            // Compute eigenvalues
            detail::symmetricEigen(col_mat.data(), dim, vectors.data(), values.data());

            return values;
        });
    }

    template <typename T, size_t N>
    inline std::pair<Serie<std::array<T, N>>, Serie<std::array<T, detail::get_matrix_dim<N>()>>>
    eigenSystem(const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenSystem requires arithmetic type");

        constexpr size_t dim = detail::get_matrix_dim<N>();

        std::vector<std::array<T, N>> vectors;
        std::vector<std::array<T, dim>> values;
        vectors.reserve(serie.size());
        values.reserve(serie.size());

        serie.forEach([&](const auto& mat, size_t) {
            std::array<T, N> col_mat;
            std::array<T, N> col_vectors;
            std::array<T, dim> eigenvalues;
            std::array<T, N> row_vectors;

            // Convert to column storage for symmetricEigen
            detail::row_to_col_symmetric<T, N>(mat.data(), col_mat.data());

            // Compute eigensystem
            detail::symmetricEigen(col_mat.data(), dim, col_vectors.data(), eigenvalues.data());

            // Convert eigenvectors back to row storage
            // detail::col_to_row_symmetric<T, N>(col_vectors.data(),
            //                                    row_vectors.data());

            vectors.push_back(row_vectors);
            values.push_back(eigenvalues);
        });

        return { Serie<std::array<T, N>>(vectors), Serie<std::array<T, dim>>(values) };
    }
    */

    // Binding functions for pipeline operations
    template <typename T, size_t N> inline auto bind_eigenVectors()
    {
        return [](const Serie<std::array<T, N>>& serie) { return eigenVectors(serie); };
    }

    template <typename T, size_t N> inline auto bind_eigenValues()
    {
        return [](const Serie<std::array<T, N>>& serie) { return eigenValues(serie); };
    }

    template <typename T, size_t N> inline auto bind_eigenSystem()
    {
        return [](const Serie<std::array<T, N>>& serie) { return eigenSystem(serie); };
    }

} // namespace df