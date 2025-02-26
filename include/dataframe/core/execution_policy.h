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
#include "ExecutionPolicy.h"
#include <iostream>

/**
 * // Example usage:
 * 
 * template<typename T>
 * Serie<T> sort_with_policy(const Serie<T>& serie, ExecutionPolicy exec) {
 *      std::vector<T> result(serie.data());
 *      auto policy = get_execution_policy(exec);
 *
 *      #if HAS_PARALLEL_ALGORITHMS
 *          std::sort(policy, result.begin(), result.end());
 *      #else
 *          std::sort(result.begin(), result.end());
 *      #endif
 *
 *      return Serie<T>(result);
 * }
 */

// Check for C++17 and above
#if __cplusplus >= 201703L

// Check specific compiler feature test macros
#if defined(__cpp_lib_parallel_algorithm) &&                                   \
    __cpp_lib_parallel_algorithm >= 201603L
#define HAS_PARALLEL_ALGORITHMS 1
#include <execution>
#else
#define HAS_PARALLEL_ALGORITHMS 0
#endif

#else
#define HAS_PARALLEL_ALGORITHMS 0
#endif

namespace df {

#if HAS_PARALLEL_ALGORITHMS

// When parallel algorithms are available
struct ExecutionPolicyTraits {
    static auto get_policy(ExecutionPolicy exec) {
        switch (exec) {
        case ExecutionPolicy::PAR:
            return std::execution::par;
        case ExecutionPolicy::PAR_UNSEQ:
            return std::execution::par_unseq;
        default:
            return std::execution::seq;
        }
    }

    static constexpr bool has_parallel_support = true;
};

#else

// Fallback when parallel algorithms are not available
struct ExecutionPolicyTraits {
    // Define a dummy policy type that works with standard algorithms
    struct dummy_policy {};
    static constexpr dummy_policy seq{};

    static dummy_policy get_policy(ExecutionPolicy) { return seq; }

    static constexpr bool has_parallel_support = false;
};

#endif

// Helper function to check at runtime if parallel algorithms are supported
constexpr bool has_parallel_algorithms() {
    return ExecutionPolicyTraits::has_parallel_support;
}

// Helper to get the actual execution policy
inline auto get_execution_policy(ExecutionPolicy exec = ExecutionPolicy::SEQ) {
    return ExecutionPolicyTraits::get_policy(exec);
}

/**
 * Helper function to check parallel support at runtime
 */
inline void print_parallel_support() {
    if (has_parallel_algorithms()) {
        std::cout << "Parallel algorithms are supported\n";
    } else {
        std::cout << "Parallel algorithms are NOT supported\n";
    }
}

} // namespace df
