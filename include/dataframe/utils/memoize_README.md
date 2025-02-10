# Memoization in DataFrame

## Overview

Memoization is an optimization technique that stores the results of expensive function calls and returns the cached result when the same inputs occur again. This implementation provides thread-safe memoization with timeout capabilities for Serie objects.

## Basic Usage

### Simple Numeric Calculations

```cpp
#include <dataframe/utils/memoize.h>

// Create a memoized function for expensive calculations
auto expensive_sqrt = df::utils::memoize([](const Serie<double>& s) {
    return s.map([](double x) {
        // Simulate expensive work
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return std::sqrt(x);
    });
});

// First call: will take time
Serie<double> data1{4.0, 9.0, 16.0};
auto result1 = expensive_sqrt(data1); // Takes ~3 seconds

// Second call with same data: instant
auto result2 = expensive_sqrt(data1); // Instant return from cache
```

### With Custom Timeout

```cpp
// Create memoized function with 30-second timeout
auto stats_calc = df::utils::memoize(
    [](const Serie<double>& s) {
        return Serie<double>{
            s.mean(),
            s.standardDeviation(),
            s.variance()
        };
    },
    std::chrono::seconds{30}
);

// Results will be cached for 30 seconds
Serie<double> data{1.0, 2.0, 3.0, 4.0, 5.0};
auto stats1 = stats_calc(data);

// Within 30 seconds: cached result
auto stats2 = stats_calc(data);

// After 30 seconds: recalculated
std::this_thread::sleep_for(std::chrono::seconds(31));
auto stats3 = stats_calc(data); // New calculation
```

## Advanced Examples

### Complex Data Processing

```cpp
// Example: Memoized feature extraction
struct FeatureVector {
    double mean;
    double stddev;
    double skewness;
    double kurtosis;
};

auto feature_extractor = df::utils::memoize([](const Serie<double>& s) {
    // Simulate complex feature extraction
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return Serie<FeatureVector>{
        FeatureVector{
            s.mean(),
            s.standardDeviation(),
            s.skewness(),
            s.kurtosis()
        }
    };
}, std::chrono::seconds{300}); // 5-minute cache

// Use in machine learning pipeline
void train_model(const Serie<double>& data) {
    for(int epoch = 0; epoch < 10; ++epoch) {
        auto features = feature_extractor(data); // Cached after first call
        // Use features for training...
    }
}
```

### Thread-Safe Usage

```cpp
// Create thread-safe memoized function
auto parallel_processor = df::utils::memoize([](const Serie<double>& s) {
    return s.map([](double x) {
        return std::pow(x, 3) + std::sqrt(x);
    });
});

// Use in multiple threads
void parallel_work(const Serie<double>& data) {
    std::vector<std::thread> threads;
    for(int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            auto result = parallel_processor(data); // Thread-safe access
            // Process result...
        });
    }
    
    for(auto& thread : threads) {
        thread.join();
    }
}
```

### Cache Management

```cpp
auto managed_calc = df::utils::memoize([](const Serie<double>& s) {
    return s.map([](double x) { return std::exp(x); });
});

// Check cache size
size_t cache_entries = managed_calc.cache_size();

// Clear cache manually if needed
managed_calc.clear_cache();

// Update timeout
managed_calc.set_timeout(std::chrono::minutes{5});
```

## Real-World Applications

### Financial Data Analysis

```cpp
// Memoized financial calculations
auto volatility_calc = df::utils::memoize([](const Serie<double>& prices) {
    // Complex volatility calculation
    auto returns = prices.map([](double p, size_t i) {
        return i > 0 ? std::log(p / prices[i-1]) : 0.0;
    });
    return returns.standardDeviation() * std::sqrt(252); // Annualized volatility
}, std::chrono::hours{1}); // 1-hour cache

// Use in trading strategy
void analyze_market(const Serie<double>& market_prices) {
    auto vol = volatility_calc(market_prices);
    if(vol > threshold) {
        // Adjust trading strategy...
    }
}
```

### Scientific Computing

```cpp
// Memoized matrix operations
auto eigenvalue_calc = df::utils::memoize([](const Serie<double>& matrix) {
    // Expensive eigenvalue calculation
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return compute_eigenvalues(matrix);
}, std::chrono::minutes{10}); // 10-minute cache

// Use in iterative solver
void solve_system(const Serie<double>& coefficient_matrix) {
    for(int iter = 0; iter < max_iters; ++iter) {
        auto eigenvals = eigenvalue_calc(coefficient_matrix);
        // Use eigenvalues in computation...
    }
}
```

## Best Practices

1. **Choose Appropriate Timeouts**
   - Consider the volatility of your data
   - Balance memory usage vs computation time
   - Use shorter timeouts for frequently changing data

2. **Memory Management**
   - Clear cache when memory is constrained
   - Monitor cache size in long-running applications
   - Consider clearing cache during low-usage periods

3. **Thread Safety**
   - Design functions to be thread-safe
   - Avoid modifying shared state in memoized functions
   - Be aware of potential deadlocks in nested memoized calls

4. **Performance Considerations**
   - Memoize only expensive computations
   - Consider the size of cached results
   - Profile your application to find optimal timeout values

## Limitations and Considerations

1. **Memory Usage**
   - Each unique input creates a cache entry
   - Large Series can consume significant memory
   - Consider clearing cache periodically

2. **Thread Contention**
   - Heavy write operations can block readers
   - Consider using smaller timeouts in high-concurrency scenarios
   - Monitor thread contention in performance-critical applications

3. **Cache Invalidation**
   - Automatic timeout-based invalidation only
   - Manual clear_cache() when data changes
   - No selective cache entry removal