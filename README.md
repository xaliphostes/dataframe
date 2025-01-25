# Dataframe

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Linux&logo=linux&logoColor=white&message=support&color=success" alt="Linux support">
  <img src="https://img.shields.io/static/v1?label=macOS&logo=apple&logoColor=white&message=support&color=success" alt="macOS support">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=soon&color=red" alt="Windows support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  
</p>

# DataFrame C++ Library

A modern C++ library (a [Panda](https://pandas.pydata.org/)-like) for efficient data manipulation and analysis, focusing on linear algebra, geometric operations, statistics, and mathematical computations.

## Core Concepts

### Serie Structure

A `GenSerie` is the fundamental data structure in the library, defined by three key parameters:

- `itemSize`: Number of components per item (e.g., 1 for scalars, 3 for 3D vectors)
- `count`: Number of items in the serie
- `dimension`: Space dimension (default is 3)

Example memory layouts:

```cpp
// Scalar serie (itemSize = 1)
GenSerie<double> scalars(1, {1,2,3,4});  // count = 4
// Memory: [s1][s2][s3][s4]

// 3D vector serie (itemSize = 3)
GenSerie<double> vectors(3, {1,2,3, 1,2,3});  // count = 2
// Memory: [x1,y1,z1][x2,y2,z2]

// Matrix (symmetric) serie (itemSize = 6)
GenSerie<double> matrices(6, {1,2,3,4,5,6,  1,2,3,4,5,6});  // count = 2
// Memory: [m11,m12,m13,m22,m23,m33][...]

// Matrix (non symmetric) serie (itemSize = 9)
GenSerie<double> matrices(9, {1,2,3,4,5,6,7,8,9,  1,2,3,4,5,6,7,8,9});  // count = 2
// Memory: [m11,m12,m13,m21,m22,m23,m31,m32,m33][...]
```

The `itemSize` determines how values are interpreted and accessed:
- `itemSize = 1`: Use `value(i)` to access single values
- `itemSize > 1`: Use `array(i)` to access component arrays

### Memory Layout

Data is stored contiguously in a single array for optimal performance. For a serie with `count = n` and `itemSize = m`:
- Total array size = `n * m`
- Item `i` starts at index `i * itemSize`
- Access item components with `array(i)[j]` where `j < itemSize`

## Features

- **Generic Serie Type**: Handle arrays of any numeric type with flexible dimensions
- **Functional Programming Style**: Support for map, reduce, filter, and forEach operations
- **Pipeline Operations**: Chain operations using intuitive pipe syntax
- **Mathematical Operations**: Built-in support for common mathematical operations
- **Type Safety**: Strong type checking and compile-time validation
- **Modern C++**: Leverages modern C++ features for clean, efficient code

## Getting Started

### Basic Usage

```cpp
#include <dataframe/Serie.h>
#include <dataframe/functional/map.h>
#include <dataframe/functional/reduce.h>

// Create a simple serie
df::Serie s1(1, {1.0, 2.0, 3.0, 4.0}); // itemSize=1, 4 elements

// Create a vector serie (3D vectors)
df::Serie vectors(3, {
    1.0, 0.0, 0.0,  // First vector
    0.0, 1.0, 0.0,  // Second vector
    0.0, 0.0, 1.0   // Third vector
});
```

### Functional Operations

#### Map Operation

```cpp
// Square each value
auto square = df::make_map([](double x, uint32_t) { return x * x; });
auto squared = square(s1);

// Vector operation
auto normalize = df::make_map([](const Array<double>& v, uint32_t) {
    double norm = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return Array<double>{v[0]/norm, v[1]/norm, v[2]/norm};
});
auto normalized = normalize(vectors);
```

#### Filter Operation

```cpp
// Filter values greater than 2
auto result = df::filter([](double x, uint32_t) { return x > 2.0; }, s1);

// Filter vectors with magnitude > 1
auto longVectors = df::filter([](const Array<double>& v, uint32_t) {
    double magSq = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
    return magSq > 1.0;
}, vectors);
```

#### Reduce Operation

```cpp
// Sum all values
double sum = df::reduce([](double acc, double val, uint32_t) {
    return acc + val;
}, s1, 0.0);

// Find the vector with maximum magnitude
auto maxVector = df::reduce([](const Array<double>& acc, 
                              const Array<double>& val, 
                              uint32_t) {
    double accMagSq = acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2];
    double valMagSq = val[0]*val[0] + val[1]*val[1] + val[2]*val[2];
    return (valMagSq > accMagSq) ? val : acc;
}, vectors, Array<double>{0,0,0});
```

### Pipeline Operations

Combine operations using the pipe operator:

```cpp
// Create operations
auto normalize = df::make_map([](const Array<double>& v, uint32_t) {
    double norm = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return Array<double>{v[0]/norm, v[1]/norm, v[2]/norm};
});

auto filterNonZero = df::make_map([](const Array<double>& v, uint32_t) {
    return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > 1e-10;
});

// Chain operations
auto result = vectors | normalize | filterNonZero;
//               ^          ^             ^
//               |          |             |
//             series      op1           op2

// Alternative using make_pipe
auto pipeline = df::make_pipe(normalize, filterNonZero);
auto result2 = pipeline(vectors);
```

## Mathematical Operations

The library includes various mathematical operations:

```cpp
// Add two series
auto sum = s1 + s2;

// Weighted sum of multiple series
auto weightedSum = df::math::weightedSum({s1, s2, s3}, {0.5, 0.3, 0.2});

// Normalize a serie
auto normalized = df::math::normalize(s1);
```

## Type Safety and Error Handling

The library provides comprehensive type checking and error handling:

```cpp
// This will throw an exception if series have different counts
auto sum = df::math::add(s1, s2);

// This will fail to compile if callback types don't match serie types
auto invalid = df::make_map([](std::string x) { return x; })(s1); // Compilation error
```

## Advanced Features

### Custom Operations

Create custom operations that can be used in pipelines:

```cpp
// Create a custom operation
auto customOp = df::make_map([](const Array<double>& v, uint32_t) {
    // Your custom computation here
    return Array<double>{/*...*/};
});

// Use it in a pipeline
auto result = vectors | customOp | normalize;
```

### Performance Considerations

- Series store data contiguously for cache-friendly access
- Operations can be chained without creating intermediate copies
- Template metaprogramming ensures compile-time optimizations

## Building and Dependencies

The library is header-only and requires:

- C++17 or later
- Standard Template Library (STL)

## License

This library is distributed under the MIT License. See LICENSE file for details.


## Contact
fmaerten@gmail.com
