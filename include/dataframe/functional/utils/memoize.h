/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <dataframe/Serie.h>
#include <map>
#include <utility>
#include <vector>

namespace df {
namespace utils {

template <typename F> class Memoized {
  public:
    Memoized(F f) : func(std::move(f)) {}

    Serie operator()(const Serie &s) const {
        auto key = make_key(s);
        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second;
        }
        auto result = func(s);
        cache[key] = result;
        return result;
    }

    void clear_cache() { cache.clear(); }

  private:
    F func;
    mutable std::map<std::vector<double>, Serie> cache;

    // Helper to create cache key from Series
    std::vector<double> make_key(const Serie &s) const {
        std::vector<double> key;
        for (uint32_t i = 0; i < s.count(); ++i) {
            if (s.itemSize() == 1) {
                key.push_back(s.template get<double>(i));
            } else {
                const auto &v = s.template get<Array>(i);
                key.insert(key.end(), v.begin(), v.end());
            }
        }
        return key;
    }
};

/**
 * @brief Memoization is a key optimization technique in computational and
scientific
 * computing, particularly valuable for expensive calculations that are
frequently
 * repeated with the same inputs. Here's a detailed explanation.
 *
 * Purpose and Benefits:
 *
 * - Performance Optimization
 *   - Caches results of expensive computations
 *   - Avoids redundant calculations
 *   - Trade-off: memory usage vs computation time
 *
 * Common Applications in Scientific Computing:
 * - Finite Element Analysis
 * - Eigenvalue Decomposition
 *
 *
 * Real-world Examples:
 * @example
 * ```cpp
// Structural Analysis
class StructuralAnalysis {
    Memoized<StiffnessCalculator> stiffness_calculator;

    void iterativeSolver() {
        for(int iter = 0; iter < max_iters; ++iter) {
            // Same stiffness matrix needed multiple times
            auto K = stiffness_calculator(strain);
            // ... solver steps
        }
    }
};

// Geomechanics
class FaultAnalysis {
    Memoized<StressAnalyzer> stress_analyzer;

    void stabilityAnalysis() {
        // Multiple scenarios with same stress states
        for(auto& scenario : scenarios) {
            auto result = stress_analyzer(initial_stress);
            // ... stability calculations
        }
    }
};
```
 *
 * Implementation Considerations:
 * - Cache Strategy
 * - Memory limits
 * Cache invalidation
 * Key generation
 * Thread Safety
 * Cache persistence
 *
 * Advanced Features:
```cpp
template<typename F>
class AdvancedMemoized {
    F func;
    mutable std::map<Key, Result> cache;
    size_t max_cache_size;
    std::chrono::seconds cache_lifetime;

    void evict_old_entries() {
        // LRU implementation
    }

    bool is_valid(const CacheEntry& entry) {
        // Check entry age
    }
};
```
 *
 * Application in Machine Learning:
```cpp
auto feature_extractor = memoize([](const Serie& data) {
    // Complex feature extraction
    return computeFeatures(data);
});

void training_loop() {
    for(int epoch = 0; epoch < epochs; ++epoch) {
        // Same features needed each epoch
        auto features = feature_extractor(training_data);
        // ... training steps
    }
}
```
 *
 * Testing and Debugging Benefits:
 * - Deterministic behavior
 * - Performance profiling
 * - Cache hit/miss analysis
 *
 * The value of memoization increases with:
 * - Computation complexity
 * - Input repetition frequency
 * - Memory availability
 * - Performance requirements
 *
 * Common in:
 * - Numerical simulations
 * - Optimization algorithms
 * - Interactive applications
 * - High-performance computing
 *
 * @example
 * ```cpp
 * auto expensive_eigenvals = memoize([](const Serie &s) {
 *      // Expensive computation
 *      return eigenValues(s);
 * });
 * ```
 */
template <typename F> auto memoize(F &&f) {
    return Memoized<std::decay_t<F>>(std::forward<F>(f));
}

} // namespace utils
} // namespace df
