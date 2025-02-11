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

#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <functional>

namespace df {

/**
 * @brief Available RBF kernel functions
 */
enum class RBFKernel {
    Gaussian,            // exp(-r²/ε²)
    Multiquadric,        // sqrt(1 + (εr)²)
    InverseMultiquadric, // 1/sqrt(1 + (εr)²)
    ThinPlate,           // r²log(r)
    Linear               // r
};

/**
 * @brief RBF kernel function implementations
 */
namespace kernels {
inline double gaussian(double r, double epsilon) {
    return std::exp(-std::pow(epsilon * r, 2));
}

inline double multiquadric(double r, double epsilon) {
    return std::sqrt(1.0 + std::pow(epsilon * r, 2));
}

inline double inverse_multiquadric(double r, double epsilon) {
    return 1.0 / std::sqrt(1.0 + std::pow(epsilon * r, 2));
}

inline double thin_plate(double r, double /*epsilon*/) {
    return r == 0 ? 0 : r * r * std::log(r);
}

inline double linear(double r, double /*epsilon*/) { return r; }
} // namespace kernels

/**
 * @brief Get kernel function based on kernel type
 */
inline std::function<double(double, double)>
get_kernel_function(RBFKernel kernel) {
    switch (kernel) {
    case RBFKernel::Gaussian:
        return kernels::gaussian;
    case RBFKernel::Multiquadric:
        return kernels::multiquadric;
    case RBFKernel::InverseMultiquadric:
        return kernels::inverse_multiquadric;
    case RBFKernel::ThinPlate:
        return kernels::thin_plate;
    case RBFKernel::Linear:
        return kernels::linear;
    default:
        throw std::runtime_error("Unknown RBF kernel type");
    }
}

} // namespace df