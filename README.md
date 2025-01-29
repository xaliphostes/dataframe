# Dataframe Library

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Linux&logo=linux&logoColor=white&message=support&color=success" alt="Linux support">
  <img src="https://img.shields.io/static/v1?label=macOS&logo=apple&logoColor=white&message=support&color=success" alt="macOS support">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=soon&color=red" alt="Windows support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  
</p>



# 
### ***<center>...Work in progress for linear algebra operations...</center>***
#

<br>

A modern C++ library for data manipulation with a focus on functional programming patterns and type safety.

## Features

- Generic series container (`Serie<T>`) for any data type
- DataFrame for managing multiple named series
- Rich functional operations (map, reduce, filter, etc.)
- Parallel processing capabilities
- Type-safe operations with compile-time checks
- Modern C++ design (C++20)

## Core Concepts

### Serie<T>

A type-safe container for sequences of data with functional operations:
- Supports any data type
- Provides functional operations (map, reduce, filter)
- Enables chaining operations using pipe syntax

### DataFrame

A container for managing multiple named series:
- Type-safe storage of different series types
- Named access to series
- Dynamic addition and removal of series

## Examples

### Basic Series Operations

```cpp
#include <dataframe/Serie.h>
#include <dataframe/map.h>
#include <dataframe/filter.h>

// Create a serie of numbers
df::Serie<int> numbers{1, 2, 3, 4, 5};

// Map operation: double each number
auto doubled = numbers.map([](int n, size_t) { return n * 2; });

// Filter operation: keep only even numbers
auto evens = numbers | df::bind_filter([](int n, size_t) { return n % 2 == 0; });

// Create a reusable pipeline using chaining operations
auto pipeline = df::bind_map([](int n, size_t) { return n * 2; }) |
                df::bind_filter([](int n, size_t) { return n > 5; });

// Apply the pipeline to the numbers serie
auto result = pipeline(numbers);
```

### Working with Custom Types

```cpp
struct Point3D {
    double x, y, z;
};

// Create a serie of 3D points
df::Serie<Point3D> points{{0,0,0}, {1,1,1}, {2,2,2}};

// Transform points
auto translated = df::map(([](const Point3D& p, size_t) {
    return Point3D{p.x + 1, p.y + 1, p.z + 1};
}, points);

// Get the norms according to (0,0,0)
auto norms = df::map(([](const Point3D& p, size_t) {
    return std::sqrt{std::pow(p.x, 2), std::pow(p.y, 2), std::pow(p.z, 2)};
}, points);
```

### DataFrame Usage

```cpp
#include <dataframe/DataFrame.h>

// Create a DataFrame
df::DataFrame df;

// Add different types of series
df.add("integers", df::Serie<int>{1, 2, 3, 4, 5});
df.add("doubles", df::Serie<double>{1.1, 2.2, 3.3, 4.4, 5.5});

// Access series with type safety
const auto& ints = df.get<int>("integers");
const auto& dbls = df.get<double>("doubles");

// Remove a series
df.remove("integers");
```

### 3D Mesh Example

```cpp
// Define types for clarity
using Point    = std::array<double, 3>;
using Triangle = std::array<uint32_t, 3>;

// Create a simple mesh
df::DataFrame mesh;

// Create vertices
df::Serie<Point> vertices{
    {0.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
};

// Create triangles
df::Serie<Triangle> triangles{
    {0, 1, 2},
    {0, 2, 3},
    {0, 3, 1},
    {1, 3, 2}
};

// Add to DataFrame
mesh.add("vertices", vertices);
mesh.add("triangles", triangles);

// Transform vertices
auto transformed_vertices = vertices.map([](const Point& p, size_t) {
    return Point{p[0] * 2.0, p[1] * 2.0, p[2] * 2.0};
});
mesh.add("transformed_vertices", transformed_vertices);
```

### Parallel Processing

```cpp
#include <dataframe/parallel_map.h>

// Process large datasets in parallel
auto result = df::parallel_map([](double x, size_t) {
    return std::sqrt(x * x + 2 * x + 1);
}, large_series);
```

## Installation

Header-only library. Simply include the headers that you need in your project.

## Requirements

- C++20 or later
- Modern C++ compiler (GCC, Clang, MSVC)

## License

MIT License - See LICENSE file for details.

## Contact
fmaerten@gmail.com
