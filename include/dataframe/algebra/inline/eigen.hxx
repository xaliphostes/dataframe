namespace df {
    namespace detail {

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
            // static_assert(
            //     triangular(value) == P, "Packed length P must be triangular (1,3,6,10,...)");
        };

        template <typename T, std::size_t N>
        inline T max_offdiag(const SymmetricMatrix<T, N>& A, std::size_t& p, std::size_t& q)
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

        // Optional helper: sort eigenpairs in descending order by value.
        template <typename T, std::size_t N>
        void sort_eigenpairs_desc(std::array<T, N>& w, FullMatrix<T, N>& V)
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
            FullMatrix<T, N> V_old = V;
            for (std::size_t j = 0; j < N; ++j) {
                std::size_t from = idx[j];
                for (std::size_t i = 0; i < N; ++i)
                    V(i, j) = V_old(i, from);
            }
        }

    } // namespace detail

    //------------------------------------------------------------------------------
    // Jacobi eigenvalue decomposition for real symmetric matrices
    // Produces: A = V * diag(w) * V^T, with V orthonormal (columns are eigenvectors)
    //------------------------------------------------------------------------------
    template <typename T, std::size_t N>
    EigenSystem<T, N> jacobi_symmetric_eigen(const SymmetricMatrix<T, N>& A_in, T tol = T(-1),
        bool sort_descending = true, std::size_t max_sweeps = 64)
    {
        // EigenSystem<T, N> out;
        SymmetricMatrix<T, N> A = A_in; // working copy
        FullMatrix<T, N> V = FullMatrix<T, N>::Identity();

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
        std::array<T, N> values;
        for (std::size_t i = 0; i < N; ++i) {
            values[i] = A(i, i);
        }

        if (sort_descending) {
            detail::sort_eigenpairs_desc(values, V);
        }

        return std::make_pair(values, V);
    }

    template <typename T, size_t N>
    Serie<std::array<T, N>> eigenValues(const Serie<SymmetricMatrix<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");

        return serie.map([](const auto& mat, size_t index) {
            auto result = jacobi_symmetric_eigen(mat);
            return result.first;
        });
    }

    template <typename T, size_t N>
    Serie<FullMatrix<T, N>> eigenVectors(const Serie<SymmetricMatrix<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenValues requires arithmetic type");

        return serie.map([](const auto& mat, size_t index) {
            auto result = jacobi_symmetric_eigen(mat);
            return result.second;
        });
    }

    template <typename T, size_t N>
    inline Serie<EigenSystem<T, N>> eigenSystem(const Serie<SymmetricMatrix<T, N>>& serie)
    {
        static_assert(std::is_arithmetic<T>::value, "eigenSystem requires arithmetic type");

        return serie.map([&](const auto& mat, size_t index) {
            return jacobi_symmetric_eigen(mat);
        });
    }
}