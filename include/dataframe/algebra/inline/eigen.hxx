namespace df {
    namespace detail {

        template <typename T, std::size_t N>
        inline constexpr std::size_t Matrix<T, N>::rows() noexcept
        {
            return N;
        }
        template <typename T, std::size_t N>
        inline constexpr std::size_t Matrix<T, N>::cols() noexcept
        {
            return N;
        }

        template <typename T, std::size_t N>
        inline constexpr T& Matrix<T, N>::operator()(std::size_t i, std::size_t j) noexcept
        {
            return d[i * N + j]; // row-major
        }

        template <typename T, std::size_t N>
        inline constexpr const T& Matrix<T, N>::operator()(
            std::size_t i, std::size_t j) const noexcept
        {
            return d[i * N + j]; // row-major
        }

        template <typename T, std::size_t N> inline Matrix<T, N> Matrix<T, N>::Identity()
        {
            Matrix I;
            for (std::size_t i = 0; i < N; ++i)
                I(i, i) = T(1);
            return I;
        }

        // Swap columns j and k (useful when reordering eigenpairs).
        template <typename T, std::size_t N>
        inline void Matrix<T, N>::swap_columns(std::size_t j, std::size_t k)
        {
            if (j == k)
                return;
            for (std::size_t i = 0; i < N; ++i)
                std::swap((*this)(i, j), (*this)(i, k));
        }

        // -----------------------------------------------------------------------------

        template <typename T, std::size_t N>
        inline T max_offdiag(const Matrix<T, N>& A, std::size_t& p, std::size_t& q)
        {
            T maxval = T(0);
            p = q = 0;
            for (std::size_t i = 0; i < N; ++i) {
                for (std::size_t j = i + 1; j < N; ++j) {
                    T v = std::abs(A(i, j));
                    if (v > maxval) {
                        maxval = v;
                        p = i;
                        q = j;
                    }
                }
            }
            return maxval;
        }

        template <typename T> inline void sym_schur2(T app, T aqq, T apq, T& c, T& s)
        {
            // Compute Jacobi rotation (c,s) that annihilates apq
            if (apq == T(0)) {
                c = T(1);
                s = T(0);
                return;
            }

            T tau = (aqq - app) / (T(2) * apq);
            T t = std::copysign(T(1), tau) / (std::abs(tau) + std::hypot(T(1), tau));
            c = T(1) / std::hypot(T(1), t);
            s = t * c;
        }

        // Row-wise packed upper-triangular index for i<=j
        template <std::size_t N> constexpr std::size_t ut_index(std::size_t i, std::size_t j)
        {
            // Start of row i in the packed upper part:
            // S(i) = i*N - i*(i-1)/2; then add (j - i)
            return i * N - (i * (i - 1)) / 2 + (j - i);
        }

        inline constexpr std::size_t triangular(std::size_t m) { return m * (m + 1) / 2; }

        inline constexpr std::size_t dim_from_packed_len(std::size_t P)
        {
            std::size_t m = 0;
            while (triangular(m) < P)
                ++m; // smallest m with T_m >= P
            return m;
        }

        template <std::size_t P> struct DimFromPacked {
            static constexpr std::size_t value = dim_from_packed_len(P);
            static_assert(
                triangular(value) == P, "Packed length P must be triangular (1,3,6,10,...)");
        };

        // -----------------------------------------------------------------------------
        // Expand packed upper-triangular array (UTP) to full symmetric Matrix<T,N>.
        // Input order matches your examples: (0,0),(0,1),...(0,N-1),(1,1),(1,2),...,(N-1,N-1).
        // -----------------------------------------------------------------------------
        // template <typename T, std::size_t N>
        // inline Matrix<T, N> from_packed_upper(const std::array<T, N*(N + 1) / 2>& packed)
        // {
        //     Matrix<T, N> A {};
        //     for (std::size_t i = 0, k = 0; i < N; ++i) {
        //         for (std::size_t j = i; j < N; ++j, ++k) {
        //             const T v = packed[k];
        //             A(i, j) = v;
        //             A(j, i) = v; // enforce symmetry
        //         }
        //     }
        //     return A;
        // }

        // Input order: (0,0),(0,1),...(0,M-1),(1,1),(1,2),...,(M-1,M-1).
        template <typename T, std::size_t P>
        [[nodiscard]] inline auto from_packed_upper(const std::array<T, P>& packed)
            -> Matrix<T, detail::DimFromPacked<P>::value>
        {
            constexpr std::size_t M = detail::DimFromPacked<P>::value;
            Matrix<T, M> A {};
            for (std::size_t i = 0, k = 0; i < M; ++i) {
                for (std::size_t j = i; j < M; ++j, ++k) {
                    const T v = packed[k];
                    A(i, j) = v;
                    A(j, i) = v; // symmetry
                }
            }
            return A;
        }

        // Optional helper: sort eigenpairs in descending order by value.
        template <typename T, std::size_t N>
        void sort_eigenpairs_desc(std::array<T, N>& w, Matrix<T, N>& V)
        {
            std::array<std::size_t, N> idx {};
            for (std::size_t i = 0; i < N; ++i)
                idx[i] = i;

            std::sort(
                idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) { return w[a] > w[b]; });

            // Apply permutation to eigenvalues
            std::array<T, N> w_sorted {};
            for (std::size_t i = 0; i < N; ++i)
                w_sorted[i] = w[idx[i]];
            w = w_sorted;

            // Apply permutation to columns of V
            Matrix<T, N> V_old = V;
            for (std::size_t j = 0; j < N; ++j) {
                std::size_t from = idx[j];
                for (std::size_t i = 0; i < N; ++i)
                    V(i, j) = V_old(i, from);
            }
        }

    } // namespace detail

    //------------------------------------------------------------------------------

    //------------------------------------------------------------------------------
    // Jacobi eigenvalue decomposition for real symmetric matrices
    // Produces: A = V * diag(w) * V^T, with V orthonormal (columns are eigenvectors)
    //------------------------------------------------------------------------------
    template <typename T, std::size_t N>
    EigenResult<T, N> jacobi_symmetric_eigen(const detail::Matrix<T, N>& A_in, T tol = T(-1),
        bool sort_descending = true, std::size_t max_sweeps = 64)
    {
        EigenResult<T, N> out;
        detail::Matrix<T, N> A = A_in; // working copy
        detail::Matrix<T, N> V = detail::Matrix<T, N>::Identity();

        // Set default tolerance if not provided
        if (!(tol > T(0))) {
            // A reasonable default: ~sqrt(eps) scaled a bit
            tol = T(10) * std::sqrt(std::numeric_limits<T>::epsilon());
        }

        std::size_t sweeps = 0;
        bool converged = false;

        for (; sweeps < max_sweeps; ++sweeps) {
            std::size_t p = 0, q = 0;
            T max_od = detail::max_offdiag(A, p, q);
            if (max_od <= tol) {
                converged = true;
                break;
            }

            // Compute rotation that zeroes A(p,q)
            T c, s;
            detail::sym_schur2(A(p, p), A(q, q), A(p, q), c, s);

            // Apply the rotation to A (both rows and columns to keep symmetry)
            T app = A(p, p), aqq = A(q, q), apq = A(p, q);
            T c2 = c * c, s2 = s * s, cs2 = T(2) * c * s;

            // Update diagonal entries
            A(p, p) = c2 * app - cs2 * apq + s2 * aqq;
            A(q, q) = s2 * app + cs2 * apq + c2 * aqq;
            A(p, q) = A(q, p) = T(0);

            // Update the rest of row/column p and q
            for (std::size_t i = 0; i < N; ++i) {
                if (i == p || i == q)
                    continue;
                T aip = A(i, p);
                T aiq = A(i, q);
                T aip_new = c * aip - s * aiq;
                T aiq_new = s * aip + c * aiq;
                A(i, p) = A(p, i) = aip_new;
                A(i, q) = A(q, i) = aiq_new;
            }

            // Accumulate eigenvectors: V := V * J(p,q,c,s)
            for (std::size_t i = 0; i < N; ++i) {
                T vip = V(i, p);
                T viq = V(i, q);
                V(i, p) = c * vip - s * viq;
                V(i, q) = s * vip + c * viq;
            }
        }

        // Fill outputs
        for (std::size_t i = 0; i < N; ++i)
            out.values[i] = A(i, i);
        out.vectors = V;

        if (sort_descending) {
            detail::sort_eigenpairs_desc(out.values, out.vectors);
        }

        out.converged = converged;
        out.sweeps = sweeps;

        return out;
    }

    template <typename T, size_t N>
    Serie<typename detail::eigen_values_info<T, N>::type> eigenValues(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");

        return serie.map([](const auto& mat, size_t index) {
            auto result = jacobi_symmetric_eigen(detail::from_packed_upper(mat));
            return result.values;
        });
    }

    template <typename T, size_t N>
    Serie<typename detail::eigen_vectors_info<T, N>::type> eigenVectors(
        const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");

        return serie.map([](const auto& mat, size_t index) {
            auto result = jacobi_symmetric_eigen(detail::from_packed_upper(mat));

            // Convert Matrix<T, M> to std::array<std::array<T, M>, M>
            constexpr std::size_t M = detail::DimFromPacked<N>::value;
            std::array<std::array<T, M>, M> vectors_array;

            for (std::size_t i = 0; i < M; ++i) {
                for (std::size_t j = 0; j < M; ++j) {
                    vectors_array[i][j] = result.vectors(i, j);
                }
            }

            return vectors_array;
        });
    }

    template <typename T, size_t N>
    inline Serie<std::pair<typename detail::eigen_values_info<T, N>::type,
        typename detail::eigen_vectors_info<T, N>::type>>
    eigenSystem(const Serie<std::array<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenSystem requires arithmetic type");

        return serie.map([&](const auto& mat, size_t index) {
            auto result = jacobi_symmetric_eigen(detail::from_packed_upper(mat));

            // Convert Matrix<T, M> to std::array<std::array<T, M>, M>
            constexpr std::size_t M = detail::DimFromPacked<N>::value;
            std::array<std::array<T, M>, M> vectors;

            for (std::size_t i = 0; i < M; ++i) {
                for (std::size_t j = 0; j < M; ++j) {
                    vectors[i][j] = result.vectors(i, j);
                }
            }

            return std::make_pair(result.values, vectors);
        });
    }
}