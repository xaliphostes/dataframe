<p align="center">
  <img src="media/logo2.png" alt="Logo dataframe" width="100">
</p>

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Linux&logo=linux&logoColor=white&message=support&color=success" alt="Linux support">
  <img src="https://img.shields.io/static/v1?label=macOS&logo=apple&logoColor=white&message=support&color=success" alt="macOS support">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=soon&color=red" alt="Windows support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
</p>

# Simple and efficient C++ Dataframe Library (header only)

<p align="center">
    <img src="media/field_comparison.jpg" alt="drawing" width="800"/>
</p>
<p align="center">
Example of <b>interpolated</b> scalar field (left: real, right: interpolation vrt to black points)

```c++
auto scattered = random_uniform<Vector2>(50, Vector2{-1.0, -1.0}, Vector2{1.0, 1.0});
auto values    = map([](const Vector2 &p) {return sin(p[0]*2) * cos(p[1]*2);}, scattered);
auto grid      = from_dims<2>({100, 100}, {0, 0}, {2.0, 2.0});
auto interp    = interpolate_field<double, 2>(grid, scattered, values);
```
</p>

<p align="center">
    <img src="media/distance_field_2d.png" alt="drawing" width="300"/>
</p>
<p align="center">
Example of <b>distance field</b> computation

```c++
auto ref_pts   = random_uniform(10, Vector2{-1.0, -1.0}, Vector2{1.0, 1.0});
auto grid      = from_dims<2>({100, 100}, {0.0, 0.0}, {2.0, 2.0});
auto distances = distance_field<2>(grid, ref_pts);
```
</p>

<p align="center">
    <img src="media/diffusion_evolution.gif" alt="drawing" width="400"/>
</p>
<p align="center">
Example of heat diffusion using <b>HarmonicDiffusion</b>
</p>


# 
### ***<center>...Work in progress for linear algebra, stats and geo(metry, logy, physic...) operations...</center>***
#

<br>

A modern C++ library for data manipulation with a focus on functional programming patterns and type safety.

Only headers. No linking!

# [Read the doc (in progress...)](https://xaliphostes.github.io/dataframe/)

## Features

- Generic series container (`Serie<T>`) for any data type (similar to a column in Excel sheet)
- DataFrame for managing multiple named series
- Rich functional operations (map, reduce, filter, etc.)
- Parallel processing capabilities
- Type-safe operations with compile-time checks
- Modern C++ design (C++23 if available)
- Use [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) for libear algebra (will install it automatically)
- Use [CGAL](https://www.cgal.org/) if needed (read [this to install CGAL](CGAL_INSTALL.md))
- Implements the functions such as `chain`, `chunk`, `compose`, `concat`, `filter`, `find`, `flatten`, `format`, `forEach`, `groupBy`, `map_if`, `map`, `memoise`, `merge`, `ones`, `orderBy`, `parallel_map`, `partition`, `pipe` with operator `|`, `print`, `range`, `reduce`, `reject`, `skip`, `slice`, `sort`, `split`, `switch`, `take`, `unique`, `unzip`, `whenAll`, `where`, `zeros`, `zip`.

## Core Concepts

### `Serie<T>`

A type-safe container for sequences of data with functional operations:
- Supports any data type
- Provides functional operations (map, reduce, filter)
- Enables chaining operations using pipe syntax

For comparison, the main difference is that while Excel columns can contain mixed types and empty cells, a Serie is strongly typed and all elements must be of the same type, making it more suitable for type-safe data processing.

### `Dataframe`

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

// Create series with default values
df::Serie<int> ints(5);        // Creates [0,0,0,0,0]
df::Serie<double> doubles(3);  // Creates [0.0,0.0,0.0]

// Create series with specific values
df::Serie<int> ones(4, 1);     // Creates [1,1,1,1]
df::Serie<double> pi(3, 3.14); // Creates [3.14,3.14,3.14]

// -------------------------------------------

// Create a serie of numbers
df::Serie<int> numbers{1, 2, 3, 4, 5};

// Map operation: double each number
// Note: "size_t index" is optional
auto doubled = numbers.map([](int n, size_t index) { return n * 2; });

// Filter operation: keep only even numbers
auto evens = numbers | df::bind_filter([](int n) { return n % 2 == 0; });

// Create a reusable pipeline using chaining operations
auto pipeline = df::bind_map([](int n) { return n * 2; }) |
                df::bind_filter([](int n) { return n > 5; });

// Apply the pipeline to the numbers serie
auto result = pipeline(numbers);
```

### Operator overloading

```cpp
#include <dataframe/Serie.h>

// Create a serie of numbers
df::Serie<int> s1{1, 2, 3, 4, 5};
df::Serie<int> s2{1, 2, 3, 4, 5};
df::Serie<int> s3{1, 2, 3, 4, 5};
df::Serie<int> s4{1, 2, 3, 4, 5};

auto s = (s1 + s2) * s3 / s4;
```

### Linear algebra

```cpp
#include <dataframe/algebra/eigen.h>
#include <dataframe/Serie.h>
#include <dataframe/types.h>

// Three sym tensor in 3D
// Row storage format, i.e., {xx, xy, xz, yy, yz, zz}
//
df::Serie<SMatrix3D> serie({
    {2, 4, 6, 3, 6, 9}, 
    {1, 2, 3, 4, 5, 6},
    {9, 8, 7, 6, 5, 4}
});

auto [values, vectors] = df::eigenSystem(serie);

df::forEach([](const EigenVectorType<3>& v) {
    std::cout << "1st eigen vector: " << v[0] << std::endl ;
    std::cout << "2nd eigen vector: " << v[1] << std::endl ;
    std::cout << "3rd eigen vector: " << v[2] << std::endl ;
}, vectors);
```

### Parallel Processing (whenAll)

The library provides several ways to perform parallel computations on Series.

The parallel processing functions are particularly useful for:
- Large datasets where computation can be distributed
- CPU-intensive operations on each element
- Processing multiple series simultaneously
- Operations that can be executed independently

Note that for small datasets, the overhead of parallel execution might outweigh the benefits. Consider using parallel operations when:
- The dataset size is large (typically > 10,000 elements)
- The operation per element is computationally expensive
- The operation doesn't require maintaining order-dependent state

```cpp
#include <dataframe/utils/whenAll.h>

// Process multiple series in parallel with transformation
df::Serie<double> s1{1.0, 2.0, 3.0, ...};
df::Serie<double> s2{4.0, 5.0, 6.0, ...};

auto result = df::whenAll([](const df::Serie<double>& s) { 
    return s.map([](double x) { return x * 2; }); 
}, {s1, s2});

// Parallel processing with tuple results
auto [r1, r2] = df::whenAll<double>(s1, s2);
```

### Working with Custom Types

```cpp
struct Point3D {
    double x, y, z;
};

// Create a serie of 3D points
df::Serie<Point3D> points{{0,0,0}, {1,1,1}, {2,2,2}};

// Transform points
auto translated = df::map(([](const Point3D& p) {
    return Point3D{p.x + 1, p.y + 1, p.z + 1};
}, points);

// Get the norms according to (0,0,0)
auto norms = df::map(([](const Point3D& p) {
    return std::sqrt{std::pow(p.x, 2), std::pow(p.y, 2), std::pow(p.z, 2)};
}, points);
```

### Dataframe Usage

```cpp
#include <dataframe/Dataframe.h>

// Create a Dataframe
df::Dataframe dataframe;

// Add different types of series
dataframe.add("integers", df::Serie<int>{1, 2, 3, 4, 5});
dataframe.add("doubles", df::Serie<double>{1.1, 2.2, 3.3, 4.4, 5.5});

// Access series with type safety
const auto& ints = dataframe.get<int>("integers");
const auto& dbls = dataframe.get<double>("doubles");

for (const auto& [name, serie] : dataframe) {
    // Work with name and serie
}

// Remove a series
dataframe.remove("integers");
```

### 3D Mesh Example

```cpp
#include <dataframe/Serie.h>
#include <dataframe/DataFrame.h>
#include <dataframe/map.h>
#include <dataframe/math/norm.h>
#include <dataframe/geo/normal.h>

// Define types for clarity
using Point    = std::array<double, 3>;
using Triangle = std::array<uint32_t, 3>;

// Create a simple mesh
df::Dataframe mesh;

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
auto transformed_vertices = df::map([](const Point& p) {
    return Point{p[0] * 2.0, p[1] * 2.0, p[2] * 2.0};
}, vertices);
mesh.add("transformed_vertices", transformed_vertices);

// Add attributes at vertices
mesh.add("norm", df::norm(vertices));
mesh.add("normal", df::normals(vertices));
```

## Installation

Header-only library. Simply include the headers that you need in your project.

## Requirements

- C++23 or later
- Modern C++ compiler (GCC, Clang, MSVC)

## License

MIT License - See LICENSE file for details.

## Contact
fmaerten@gmail.com
