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

#include <Eigen/Dense>

namespace df {

template <typename T> inline Serie<T> solve(const Serie<T> &A, const Serie<T> &b) {
    // Check if A is square (n x n) and b is n x 1
    size_t n = static_cast<size_t>(std::sqrt(A.size()));
    if (n * n != A.size()) {
        throw std::invalid_argument("Matrix A must be square");
    }
    if (b.size() != n) {
        throw std::invalid_argument(
            "Vector b size must match matrix dimension");
    }

    // Convert Serie to Eigen types
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> eigenA(n, n);
    Eigen::Vector<T, Eigen::Dynamic> eigenB(n);

    // Fill matrix A
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            eigenA(i, j) = A[i * n + j];
        }
        eigenB(i) = b[i];
    }

    // Solve using Eigen
    Eigen::Vector<T, Eigen::Dynamic> x =
        eigenA.colPivHouseholderQr().solve(eigenB);

    // Convert back to Serie
    std::vector<T> result(n);
    for (size_t i = 0; i < n; ++i) {
        result[i] = x(i);
    }

    return Serie<T>(result);
}

// Helper function for pipe operations
template <typename T> inline auto bind_solve(const Serie<T> &b) {
    return [&b](const Serie<T> &A) { return solve(A, b); };
}

} // namespace df