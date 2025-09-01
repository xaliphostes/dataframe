#include <cmath>
#include <dataframe/algebra/dot.h>
#include <dataframe/algebra/eigen.h>
#include <dataframe/algebra/norm.h>
#include <dataframe/core/forEach.h>
#include <dataframe/core/pipe.h>
#include <dataframe/math/weightedSum2.h>
#include <dataframe/types.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace df;

/**
 * Function to compute colinearity between two eigenvector sets
 * Returns the absolute value of dot product (cosine of angle between vectors)
 * Value close to 1.0 indicates high colinearity (parallel/anti-parallel)
 * Value close to 0.0 indicates orthogonality
 */
template <size_t N>
double computeColinearity(const std::array<Vector3, N> &eigenvecs1,
                          const std::array<Vector3, N> &eigenvecs2,
                          size_t vec_index) {
    const auto &v1 = eigenvecs1[vec_index];
    const auto &v2 = eigenvecs2[vec_index];

    // Compute dot product
    double dot_product = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];

    // Return absolute value to handle both parallel and anti-parallel cases
    return std::abs(dot_product);
}

/**
 * Function to compute angle between two vectors in degrees
 */
template <size_t N>
double computeAngle(const std::array<Vector3, N> &eigenvecs1,
                    const std::array<Vector3, N> &eigenvecs2,
                    size_t vec_index) {
    const auto &v1 = eigenvecs1[vec_index];
    const auto &v2 = eigenvecs2[vec_index];

    // Compute dot product
    double dot_product = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];

    // Clamp to [-1, 1] to avoid numerical issues with acos
    dot_product = std::max(-1.0, std::min(1.0, dot_product));

    // Return angle in degrees
    return std::acos(std::abs(dot_product)) * 180.0 / M_PI;
}

int main() {
    std::cout << "=== Weighted Sum of Symmetric Matrices with Eigenvector "
                 "Analysis ==="
              << std::endl;
    std::cout << std::fixed << std::setprecision(6);

    // Create measured stress matrix (3x3 symmetric)
    // Using row-symmetric storage: [a11, a12, a13, a22, a23, a33]
    Serie<SMatrix3D> measured{
        {10.0, 2.0, 1.0, 8.0, 1.5, 6.0} // Measured stress tensor
    };

    std::cout << "\n1. Measured Stress Matrix:" << std::endl;
    std::cout << "   Matrix components (row-symmetric): ";
    for (size_t i = 0; i < 6; ++i) {
        std::cout << measured[0][i] << " ";
    }
    std::cout << std::endl;

    // Create six computed stress matrices (Stress1 to Stress6)
    std::vector<Serie<SMatrix3D>> computed_stresses = {
        Serie<SMatrix3D>{{12.0, 1.8, 0.8, 9.0, 1.2, 7.0}}, // Stress1
        Serie<SMatrix3D>{{8.5, 2.5, 1.5, 7.5, 1.8, 5.5}},  // Stress2
        Serie<SMatrix3D>{{11.0, 1.5, 0.5, 8.5, 1.0, 6.5}}, // Stress3
        Serie<SMatrix3D>{{9.8, 2.2, 1.2, 7.8, 1.6, 5.8}},  // Stress4
        Serie<SMatrix3D>{{10.5, 1.9, 0.9, 8.2, 1.3, 6.2}}, // Stress5
        Serie<SMatrix3D>{{9.2, 2.4, 1.4, 7.2, 1.7, 5.7}}   // Stress6
    };

    std::cout << "\n2. Computed Stress Matrices:" << std::endl;
    for (size_t i = 0; i < computed_stresses.size(); ++i) {
        std::cout << "   Stress" << (i + 1) << ": ";
        for (size_t j = 0; j < 6; ++j) {
            std::cout << computed_stresses[i][0][j] << " ";
        }
        std::cout << std::endl;
    }

    // Define weight array (6 weights for 6 computed matrices)
    std::vector<double> weights = {0.2, 0.15, 0.25, 0.1, 0.2, 0.1};

    std::cout << "\n3. Weights for Weighted Sum:" << std::endl;
    std::cout << "   Weights: ";
    for (size_t i = 0; i < weights.size(); ++i) {
        std::cout << weights[i] << " ";
    }
    std::cout << std::endl;

    // Verify weights sum to 1.0
    double weight_sum = 0.0;
    for (double w : weights)
        weight_sum += w;
    std::cout << "   Weight sum: " << weight_sum << std::endl;

    // Compute weighted sum of computed stress matrices
    Serie<SMatrix3D> weighted_result = weightedSum(computed_stresses, weights);

    std::cout << "\n4. Weighted Sum Result:" << std::endl;
    std::cout << "   Weighted sum: ";
    for (size_t i = 0; i < 6; ++i) {
        std::cout << weighted_result[0][i] << " ";
    }
    std::cout << std::endl;

    // Compute eigenvalues and eigenvectors for measured matrix
    auto [measured_eigenvecs, measured_eigenvals] = eigenSystem(measured);

    std::cout << "\n5. Measured Matrix Eigen Analysis:" << std::endl;
    std::cout << "   Eigenvalues: ";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << measured_eigenvals[0][i] << " ";
    }
    std::cout << std::endl;

    std::cout << "   Eigenvectors:" << std::endl;
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "     v" << (i + 1) << ": [" << measured_eigenvecs[0][i][0]
                  << ", " << measured_eigenvecs[0][i][1] << ", "
                  << measured_eigenvecs[0][i][2] << "]" << std::endl;
    }

    // Compute eigenvalues and eigenvectors for weighted sum result
    auto [weighted_eigenvecs, weighted_eigenvals] =
        eigenSystem(weighted_result);

    std::cout << "\n6. Weighted Sum Eigen Analysis:" << std::endl;
    std::cout << "   Eigenvalues: ";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << weighted_eigenvals[0][i] << " ";
    }
    std::cout << std::endl;

    std::cout << "   Eigenvectors:" << std::endl;
    for (size_t i = 0; i < 3; ++i) {
        std::cout << "     v" << (i + 1) << ": [" << weighted_eigenvecs[0][i][0]
                  << ", " << weighted_eigenvecs[0][i][1] << ", "
                  << weighted_eigenvecs[0][i][2] << "]" << std::endl;
    }

    // Compare eigenvector colinearity between measured and weighted sum
    std::cout << "\n7. Eigenvector Colinearity Analysis:" << std::endl;
    std::cout
        << "   (Colinearity: 1.0 = parallel/anti-parallel, 0.0 = orthogonal)"
        << std::endl;

    for (size_t i = 0; i < 3; ++i) {
        double colinearity = computeColinearity<3>(measured_eigenvecs[0],
                                                   weighted_eigenvecs[0], i);
        double angle =
            computeAngle<3>(measured_eigenvecs[0], weighted_eigenvecs[0], i);

        std::cout << "   Eigenvector " << (i + 1) << ":" << std::endl;
        std::cout << "     Colinearity: " << colinearity << std::endl;
        std::cout << "     Angle: " << angle << "°" << std::endl;
    }

    // Compute eigenvalues and eigenvectors for each individual computed matrix
    std::cout << "\n8. Individual Computed Matrix Colinearity with Measured:"
              << std::endl;

    for (size_t mat_idx = 0; mat_idx < computed_stresses.size(); ++mat_idx) {
        auto [computed_eigenvecs, computed_eigenvals] =
            eigenSystem(computed_stresses[mat_idx]);

        std::cout << "   Stress" << (mat_idx + 1)
                  << " vs Measured:" << std::endl;

        for (size_t vec_idx = 0; vec_idx < 3; ++vec_idx) {
            double colinearity = computeColinearity<3>(
                measured_eigenvecs[0], computed_eigenvecs[0], vec_idx);
            double angle = computeAngle<3>(measured_eigenvecs[0],
                                           computed_eigenvecs[0], vec_idx);

            std::cout << "     Eigenvector " << (vec_idx + 1)
                      << " - Colinearity: " << colinearity
                      << ", Angle: " << angle << "°" << std::endl;
        }
    }

    // Summary analysis
    std::cout << "\n9. Summary Analysis:" << std::endl;

    // Compute overall colinearity score (average of all three eigenvector
    // colinearities)
    double overall_colinearity = 0.0;
    for (size_t i = 0; i < 3; ++i) {
        overall_colinearity += computeColinearity<3>(measured_eigenvecs[0],
                                                     weighted_eigenvecs[0], i);
    }
    overall_colinearity /= 3.0;

    std::cout << "   Overall colinearity score (weighted sum vs measured): "
              << overall_colinearity << std::endl;

    if (overall_colinearity > 0.9) {
        std::cout << "   → Excellent alignment between measured and weighted "
                     "sum eigenvectors"
                  << std::endl;
    } else if (overall_colinearity > 0.7) {
        std::cout << "   → Good alignment between measured and weighted sum "
                     "eigenvectors"
                  << std::endl;
    } else if (overall_colinearity > 0.5) {
        std::cout << "   → Moderate alignment between measured and weighted "
                     "sum eigenvectors"
                  << std::endl;
    } else {
        std::cout << "   → Poor alignment between measured and weighted sum "
                     "eigenvectors"
                  << std::endl;
    }

    // Find the computed matrix with best overall colinearity to measured
    size_t best_matrix_idx = 0;
    double best_colinearity = 0.0;

    for (size_t mat_idx = 0; mat_idx < computed_stresses.size(); ++mat_idx) {
        auto [computed_eigenvecs, computed_eigenvals] =
            eigenSystem(computed_stresses[mat_idx]);

        double mat_colinearity = 0.0;
        for (size_t vec_idx = 0; vec_idx < 3; ++vec_idx) {
            mat_colinearity += computeColinearity<3>(
                measured_eigenvecs[0], computed_eigenvecs[0], vec_idx);
        }
        mat_colinearity /= 3.0;

        if (mat_colinearity > best_colinearity) {
            best_colinearity = mat_colinearity;
            best_matrix_idx = mat_idx;
        }
    }

    std::cout << "   Best individual matrix: Stress" << (best_matrix_idx + 1)
              << " (colinearity: " << best_colinearity << ")" << std::endl;

    return 0;
}