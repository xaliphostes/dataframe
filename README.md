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
GenSerie<double> scalars(1, 4);  // count = 4
// Memory: [s1][s2][s3][s4]

// 3D vector serie (itemSize = 3)
GenSerie<double> vectors(3, 2);  // count = 2
// Memory: [x1,y1,z1][x2,y2,z2]

// Matrix serie (itemSize = 9)
GenSerie<double> matrices(9, 2);  // count = 2
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
auto normalize = df::make_map([](const std::vector<double>& v, uint32_t) {
    double norm = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return std::vector<double>{v[0]/norm, v[1]/norm, v[2]/norm};
});
auto normalized = normalize(vectors);
```

#### Filter Operation

```cpp
// Filter values greater than 2
auto result = df::filter([](double x, uint32_t) { return x > 2.0; }, s1);

// Filter vectors with magnitude > 1
auto longVectors = df::filter([](const std::vector<double>& v, uint32_t) {
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
auto maxVector = df::reduce([](const std::vector<double>& acc, 
                              const std::vector<double>& val, 
                              uint32_t) {
    double accMagSq = acc[0]*acc[0] + acc[1]*acc[1] + acc[2]*acc[2];
    double valMagSq = val[0]*val[0] + val[1]*val[1] + val[2]*val[2];
    return (valMagSq > accMagSq) ? val : acc;
}, vectors, std::vector<double>{0,0,0});
```

### Pipeline Operations

Combine operations using the pipe operator:

```cpp
// Create operations
auto normalize = df::make_map([](const std::vector<double>& v, uint32_t) {
    double norm = std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return std::vector<double>{v[0]/norm, v[1]/norm, v[2]/norm};
});

auto filterNonZero = df::make_map([](const std::vector<double>& v, uint32_t) {
    return std::sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]) > 1e-10;
});

// Chain operations
auto result = vectors | normalize | filterNonZero;

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

### Eigen Operations
```cpp
// Create a 3x3 symmetric matrix serie
// Order is (xx, xy, xz, yy, yz, zz)
df::Serie serie(6, {
    2, 4, 6, 3, 6, 9, // First stress 
    1, 2, 3, 4, 5, 6, // Second stress
    9, 8, 7, 6, 5, 4  // Third stress
});

auto [values, vectors] = df::algebra::eigenSystem(serie);
df::print(values);
df::print(vectors);
```
Will display:
```
GenSerie<double> {
  itemSize : 3
  count    : 3
  dimension: 3
  values   : [
    [16.3328, -0.6580, -1.6748], // eigen values for the first stress
    [11.3448, 0.1709, -0.5157],
    [20.1911, -0.0431, -1.1480]
  ]
}
GenSerie<double> {
  itemSize : 9
  count    : 3
  dimension: 3
  values   : [
    // eigen vectors for the first stress (v1x, v1y, v1z, v2x, v2y, v2z, v3x, v3y, v3z)
    [0.4493, 0.4752, 0.7565, 0.1945, 0.7745, -0.6020, 0.8720, -0.4176, -0.2556],
    [0.3280, 0.5910, 0.7370, -0.5921, 0.7365, -0.3271, 0.7361, 0.3291, -0.5915],
    [0.6888, 0.5534, 0.4683, 0.1594, -0.7457, 0.6469, -0.7072, 0.3709, 0.6019]
  ]
}
```

## Type Safety and Error Handling

The library provides comprehensive type checking and error handling:

```cpp
// This will throw an exception if series have different counts
auto sum = df::math::add(s1, s2);

// This will fail to compile if callback types don't match serie types
auto invalid = df::make_map([](std::string x) { return x; })(s1); // Compilation error
```

## Working with Custom Types

```cpp
// Define a custom type
struct Position {
    double x, y, z;
    
    // Optional: Operators for serie operations
    Position operator+(const Position& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    
    Position operator*(double scale) const {
        return {x * scale, y * scale, z * scale};
    }
};

std::ostream &operator<<(std::ostream &o, const Position &p) {
    o << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return o;
}

// Direct instantiation with custom types
df::GenSerie<Position> positions(1, {
    Position{1, 0, 0},
    Position{0, 2, 0},
    Position{0, 0, 3}
});

// Operations work naturally with the custom type
auto doubled = df::make_map([](const Position& p, uint32_t) {
    return p * 2.0;
})(positions);

// Filter based on custom type properties
auto filtered = df::filter([](const Position& p, uint32_t) {
    return (p.x*p.x + p.y*p.y + p.z*p.z) < 2.0;
}, positions);
```

## Advanced Features

### Custom Operations

Create custom operations that can be used in pipelines:

```cpp
// Create a custom operation
auto customOp = df::make_map([](const std::vector<double>& v, uint32_t) {
    // Your custom computation here
    return std::vector<double>{/*...*/};
});

// Use it in a pipeline
auto result = vectors | customOp | normalize;
```

## Full examples

### Stress state
```cpp
#include <dataframe/Serie.h>
#include <dataframe/functional/filter.h>
#include <dataframe/functional/utils/reject.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/map.h>
#include <map>

using namespace df;

int main() {
    // Stress tensor components (xx,xy,xz,yy,yz,zz)
    GenSerie<double> stress(6, {
        1,  0,  0, 1,  0, 1,  // Point 1
        2,  1,  0, 2,  0, 2,  // Point 2
        -1, 0,  0, -1, 0, -1, // Point 3
        -2, -1, 0, -2, 0, -2  // Point 4
    });

    GenSerie<double> distances(1, {5.0, 2.0, 1.0, 0.5});
    GenSerie<int> rockTypes(1, {1, 1, 2, 2});

    // Process data pipeline
    auto filtered = pipe(
        zip(stress, distances, rockTypes),

        // Reject points far from faults
        make_reject([](const std::vector<double>& data, uint32_t) {
            return data[6] > 2.0;  // distance is after stress components
        }),

        // Keep only compressive states
        make_filter([](const std::vector<double>& data, uint32_t) {
            double trace = data[0] + data[3] + data[5];  // xx + yy + zz
            return trace < 0;
        })
    );

    // Compute average stress by rock type
    std::map<int, std::pair<std::vector<double>, int>> averages;
    
    filtered.forEach([&averages](const std::vector<double>& data, uint32_t) {
        int rockType = static_cast<int>(data[7]);
        auto& [sum, count] = averages[rockType];

        if (count == 0) {
            sum = std::vector<double>(data.begin(), data.begin() + 6);
        } else {
            for (int i = 0; i < 6; i++) {
                sum[i] = (sum[i] * count + data[i]) / (count + 1);
            }
        }
        count++;
    });

    return 0;
}
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
