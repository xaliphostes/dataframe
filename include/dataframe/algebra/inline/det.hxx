namespace df {

    template <typename T, std::size_t N> inline Serie<T> det(const Serie<FullMatrix<T, N>>& serie)
    {
        return serie.map(
            [](const FullMatrix<T, N>& matrix, size_t) -> T { return matrix.determinant(); });
    }

    template <typename T, std::size_t N>
    inline Serie<T> det(const Serie<SymmetricMatrix<T, N>>& serie)
    {
        return serie.map(
            [](const SymmetricMatrix<T, N>& matrix, size_t) -> T { return matrix.determinant(); });
    }

    template <typename T, std::size_t N> inline auto bind_det_fullmatrix()
    {
        return [](const Serie<FullMatrix<T, N>>& serie) { return det(serie); };
    }

    template <typename T, std::size_t N> inline auto bind_det_symmetric()
    {
        return [](const Serie<SymmetricMatrix<T, N>>& serie) { return det(serie); };
    }

    template <typename MatrixType> inline auto bind_det()
    {
        return [](const Serie<MatrixType>& serie) { return det(serie); };
    }

}