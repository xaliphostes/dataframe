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

Minimalist [Panda](https://pandas.pydata.org/)-like library in C++ which allows you to create and manage series and dataframes. It also provides math functions to play with Series.

Compared to Panda, we have some differences since our main consern is [**linear algebra**](https://en.wikipedia.org/wiki/Linear_algebra).

## Core concepts
A Serie is a fundamental concept in linear algebra and numerical computation that represents a collection of mathematical objects that share the same structure. Each item in a Serie can be:
- A scalar value (itemSize=1)
- A vector (itemSize>1, e.g., 2D or 3D vectors)
- A tensor (e.g., stress tensor with itemSize=6 for symmetric or 9 for full)

Even if this lib is called `dataframe` (like the **Panda** lib in Python), the main class is `Serie`.
<br>The `Dataframe` class is only used to store multiple series with their associated name (i.e., a `std::map` of Serie) 

Key operations include:
```cpp
// Core transformations
map()      // Transform each item via a function 
reduce()   // Aggregate items into a single value/vector
filter()   // Select items meeting a condition

// Transformation/execution
parallel_execute()
memoize()

// Linear algebra
eigenValues()    // Compute eigenvalues of tensor items
eigenVectors()   // Compute eigenvectors of tensor items
normalize()      // Normalize vectors or tensors
inv()           // Invert tensors

// Combinations
zip()      // Combine multiple Series element-wise
forEach()  // Process each item without modifying
```
Series enable vector/tensor operations to be expressed naturally as transformations on collections of mathematical objects, maintaining their algebraic properties throughout the computations.

Example workflow:
```cpp
auto stresses = Serie(6, {...});  // Symmetric stress tensors
auto [values, vectors] = eigenSystem(stresses);  // Decompose into principal values/directions
auto critical = filter(values, [](const Array& v) {
    return v[0] < 0;  // Find compressive states
});
```

A more complete workflow using the `pipe` function is given below:
```cpp
/**
 * Pipeline to:
 *  1. Calculate eigenvalues/vectors
 *  2. Filter compressive states (first eigenvalue < 0)
 *  3. Normalize eigenvectors
 *  4. Compute final measure
 */
auto critical_stress_pipeline = make_pipe(
   [](const Serie& s) {
       auto [val, vec] = eigenSystem(s);
       return zip(val, vec, positions);  // Combine all data
   },
   filter([](const Array& data, uint32_t) {
       auto eigenvals = Array(data.begin(), data.begin() + 3);
       return eigenvals[0] < 0;  // Keep compressive states
   }),
   map([](const Array& data, uint32_t) {
       auto pos = Array(data.end() - 3, data.end());
       return norm(pos);  // Return distance from origin
   })
);

/**
 * Usage of the pipeline:
 */
auto stresses = Serie(6, {...}); // stresses at n points in 3D-space
auto critical_stress = critical_stress_pipeline(stresses);
```

How to run an algorithm in parallel (excerpt from `example/parallel-postprocess/main.cxx`)
```cpp
#include <dataframe/functional/geo/cartesian_grid.h>
#include <dataframe/functional/partition_n.h>
#include <dataframe/functional/parallel_execute.h>

/**
 * Computes the 3D Green's function (elastic fundamental solution) for a
 * point force in an infinite elastic medium and return the stress tensor.
 */
class Source {
  public:
    Source(const Array &pos, const Array force);
    Stress stress(const Array &at);
  private:
    double xs{0}, ys{0}, zs{0};    // Source position
    double fx{1000}, fy{0}, fz{0}; // Source force
    double nu{0.25};               // poisson's ratio
    double mu{1};                  // shear modulus
};

/**
 * Definition of the functor (which cumulate the stress due to each source
 * point) and which will be run in parallel on multiple cores.
 */
struct Model {
    Model(u_int32_t nbSources = 1e4);

    df::Serie operator()(const df::Serie &points) const; // <==========
};

int main() {
    u_int32_t nbSources = 1e4;
    u_int32_t nbFields = 1e6;
    uint nbCores = 12;

    Model model(nbSources);

    // An observation grid around the model
    df::Serie grid = df::grid::cartesian::from_points(
        {100, 100, 100}, {-10, -10, -10}, {10, 10, 10});

    // Parallelize the computation
    auto strain = df::parallel_execute(model, df::partition_n(nbCores, grid));
}
```
___
<br><br><br>

A Serie is based on 4 values:
- `itemSize()`: the elementary size of the item in a Serie
- `count()`: the number of items in a Serie
<br><br>
- `size()`: the number of `double` stored in a Serie 
- `dimension()`: the space dimension of the Serie. Can be either 2 dimensional or 3 dimensional.

and major constructors are:
- `Serie(itemSize, Array, dimension=3)`
- `Serie(itemSize, count, dimension=3)`

A Serie can be defined for scalars, vectors or matrices:

### Scalar
For the scalar case, the corresponding itemSize is 1.

For example:
```cpp
auto s = Serie(1, {1,0,1,-1,0,-1}); // itemSize=1, count = 6
```

### Vector
For vectors, it is the size of the store elementary vectors (all vectors have the same size in a Serie).

For example:
- 2D coordinate vector: `(x, y)` (itemSize=2)
```cpp
// 3 points in a 2D-space
auto s = Serie(2, {0,0,  1,0,  3,1}, 2); // itemSize=2, count = 3
```
- 3D coordinate vector: `(x, y, z)` (itemSize=3)
```cpp
// 2 points in a 3D-space
auto s = Serie(3, {0,0,0,  1,0,0}); // itemSize=3, count = 2
```
- any vector size
```cpp
auto s = Serie(4, {0,0,0,0,  1,0,1,0}); // itemSize=4, count = 2
```

### Matrix
For matrices, they are stored as packed arrays, and the size depends on the dimention as well as the rank.

For example:
- 2D, symmetric matrix: `(xx, xy, yy)` (itemSize=3)
```cpp
// Here, we provide the dimension (default is 3), we never know...
auto s = Serie(3, {1,0,1,  -1,0,-1}, 2); // itemSize=3, count = 2, dimension=2
```
- 2D, non-symmetric matrix: `(xx, xy, yx, yy)` (itemSize=4)
```cpp
auto s = Serie(4, {1,0,1,2,  -1,0,-1,2}); // itemSize=4, count = 2
```
- 3D, symmetric matrix: `(xx, xy, xz, yy, yz, zz)` (itemSize=6)
```cpp
auto s = Serie(6, {1,0,1,2,0,3,  -1,0,-1,2,3,2}); // itemSize=6, count = 2
```
- 3D, non-symmetric matrix: `(xx, xy, xz, yx,yy, yz, zx, zy, zz)` (itemSize=9)
```cpp
auto s = Serie(9, {1,0,1,2,0,3,2,3,1,  -1,0,-1,2,3,2,-3,3,2}); // itemSize=9, count = 2
```

The `size()` is rarely used and represents `size = itemSize * count`

The `dimension()` is often used internaly for discriminating between special cases. For example, a symmetric tensor in 2D has `itemSize=3`, i.e., `(xx, xy, yy)`. However, a vector in 3D also has `itemSize=3`, i.e., `(x, y, z)`.

## Requirements
- C++20 (but C++23 soon for parallelization (specifically on macos), by using `std::execution::par_unseq`)
- cmake

Main functionalities are:
- Compared to Panda, each item of a Serie is either a scalar (i.e., a number) or an array of scalars... and that's it!. This is it because our main concern is [**linear algebra**](https://en.wikipedia.org/wiki/Linear_algebra). Tensors are stored as packed array, hense the notion of `itemSize`.
    - A first example is given by a `itemSize=6` with `dimension=3` which represents a symmetric matrix of dimension 3 (6 coefficients)
    - A second exmaple is given by a `itemSize=3` with `dimension=3` which represents a vector of dimension 3 (3 coefficients)
    - However, the same last example but with `itemSize=2` represents a matrix of dimension 2 (still 3 coefficients).
    - When `itemSize=9` and `dimension=3`, we have a non-symmetric matrix of dimension 3 (9 coefficients)

- `Serie` of scalar items or n-dim items
- Mathematical functions on `Serie`
- Decomposition of a `Serie` into a new `Serie` (virtual Serie) using the `Manager` and some `Decomposer`s
- Functional programming (no mutating, less bugs, very simple to use)
- Very simple API for both `Serie` and `Dataframe`
- Can be easily extended for mathematical functions such as **eigenValues** or **eigenVectors** on a `Serie`

Not yet tested under Windows, but will have to add `export` for shared library.

# Organization of this lib

- Main files are `Dataframe` and `Serie`
- Folders:
    - **algos** provides some algorithms such as:
        - `filter`
        - `foreach`
        - `zip`
        - `pipe`
        - `reduce`
        - `map`
        - ...
    - **operations** provides some examples of what is possible to do with `Serie` in term of linear algebra, statistics etc... (see also the [dataframe in TypeScript](https://github.com/youwol/dataframe)). It also provides some examples of functions to create `Serie`s from geometry, geophysics and so on... Right now, sub folders are
        - `algebra`
            - `cross`
            - `det`
            - `inv`
            - `dot`
            - `eigen`
            - `norm`
            - `transpose`
            - ...
        - `geo` (for geometry, geology, geophysics...)
            - `area`
            - `insar`
            - `normals`
            - ...
        - `math`
            - `add`
            - `div`
            - `equals`
            - `minMax`
            - `mult`
            - `negate`
            - `scale`
            - `sub`
            - `weightedSum`
            - ...
        - `stats`
            - `bins`
            - `covariance`
            - `mean`
            - `quantile`
            - ...
    - **attributes** provides a way of decomposing any `Serie` into other `Serie`. For example, a `Serie` with `itemType=6` might represent symmetric matrices 3x3. Therefore, attributes (i.e., possible decomposed `Serie`s) can be :
        - Components of the matrices
        - Eigen vectors
        - Eigen values
        - ...

# Compilation
Create a `build` directory, **go inside** and type
```sh
cmake .. && make -j12
```

# Running unit tests
**NOTE**: The internal cmake test is used to perform unit testing.

In the **same directory** as for the compilation (i.e., the `build` directory), only type
```sh
ctest
```
or
```sh
make test
```

# Warning
When including algos from `<dataframe/functional/>`, be sure to include first `<dataframe/Serie.h>`!

# API

## Example: forEach
```cpp
df::Serie s1(1, {10, 20});           // scalars
df::Serie s2(2, {1, 2, 3, 4});       // 2D vectors
df::Serie s3(3, {1, 2, 3, 4, 5, 6}); // 3D vectors

// Loop over s2 only
df::forEach(s2, [](const Array &v, uint32_t i) {
    std::cout << "Vector2 " << i << ": " << v << "\n";
});

// Loop over s1, s2 and s3 together
df::forEach(
    [=](const Array &v1, const Array &v2, const Array &v3, uint32_t i) {
        std::cout << "Index " << i << ":\n"
                << "  v1 = " << v1
                << "  v2 = " << v2
                << "  v3 = " << v3 << std::endl;
    },
    s1, s2, s3
);
```
will display
```
Index 0:
  v1 = 10 
  v2 = 1 2 
  v3 = 1 2 3 

Index 1:
  v1 = 20 
  v2 = 3 4 
  v3 = 4 5 6
```

## Example: dot product 2 series
```cpp
df::Serie a(3, {1,2,5,  3,4,9}) ; // first param is the item size of the Serie: 3
df::Serie b(3, {4,3,3,  2,1,0}) ;

df::Serie dot = df::dot(a, b) ;
std::cout << dot << std::endl;
```

## Example: weighted sum
Performs a weighted sum of Series ;-)

Constraints:
- All `Serie` must have the same `count()`
- All `Serie` must have the same `itemSize()`
- Size of weights must be equal to the size of array of the provided `Serie`s
```cpp
df::Serie a(2, {1,2,  3,4}) ;
df::Serie b(2, {4,3,  2,1}) ;
df::Serie c(2, {2,2,  1,1}) ;

// Performs s = 2*a + 3*b + 4*c
auto s = df::weigthedSum({a, b, c}, {2, 3, 4}) ;
std::cout << s << std::endl;
```
will display
```
Serie:
  itemSize : 2
  count    : 2
  dimension: 3
  values   : [22, 21, 16, 15]
```

## Example: eigen vectors
Eigen
```cpp
df::Serie s(6, {....}) ; // symmetric 3x3 matrices => 6 coefs
auto vectors = df::eigenVectors(s) ; // and that's it!

// Display
vectors.forEach([](const Array& v, uint32_t index) {
    std::cout << "eigen vector at index " 
              << std::to_string(index)
              << " is: " << v  << std::endl ;
});
```

## Example: chaining...
```cpp
df::Serie a(2, {1,2,  3,4}) ;
df::Serie b(2, {4,3,  2,1}) ;
df::Serie c(2, {2,2,  1,1}) ;

auto s = df::weigthedSum({a, b, c}, {2, 3, 4})
    .map([](const Array& arr, uint32_t) {
        return std::sqrt( pow(arr[0],2) + pow(arr[1],2) + pow(arr[2],2) );
    })
    .forEach([](double v, uinit32_t index) {
        std::cout << index << ": " << v << std::endl ;
    }) ;
```
or using the `pipe` function:
```cpp
Serie a(2, {1,2,  3,4});
Serie b(2, {4,3,  2,1});
Serie c(2, {2,2,  1,1});

pipe(
    weigthedSum({a, b, c}, {2, 3, 4}),

    make_map([](const Array& arr, uint32_t) {
        return std::sqrt(pow(arr[0],2) + pow(arr[1],2) + pow(arr[2],2));
    }),

    make_forEach([](double v, uint32_t index) {
        std::cout << index << ": " << v << std::endl;
    })
);
```

## Example: attributes
```cpp
df::Dataframe dataframe;
dataframe.add("positions", Serie(3, {...})); // geometry
dataframe.add("indices", Serie(3, {...})); // topology (triangles)
dataframe.add("S", Serie(6, {...})); // Stresses (sym 3x3 matrices)

df::Manager mng(dataframe, {
    Coordinates(),
    Components(),
    EigenValues(),
    EigenVectors(),
    Normals("n")
])

// Gather all possible scalar serie names
console.log( mng.names(1) )
// Will display: Sxx, Sxy, Sxz, Syy, Syz, Szz, S1, S2, S3, x, y, z, nx, ny, nz

// Gather all possible vector serie names
console.log( mng.names(3) )
// Will display: S1, S2, S3, n

// Gather all possible symmetric matrix serie names
console.log( mng.names(6) )
// Will display: S

Serie scalarS1 = mng.serie(1, 'S1') // eigen value S1 for all items
Serie vectorS1 = mng.serie(3, 'S1') // eigen vector S1 for all items
```

## Example: piping
A complete example using multiple features using the piping feature

```cpp
#include <dataframe/Serie.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/zip.h>
#include <dataframe/functional/map.h>
#include <dataframe/functional/filter.h>
#include <dataframe/functional/forEach.h>
#include <dataframe/functional/algebra/eigen.h>
#include <dataframe/functional/stats/mean.h>

using namespace df;

double cohesion = 0.1;
double friction_angle = 30 * M_PI / 180;

// Compute the critical stress state
auto computeCriticalityIndex = [=](const Serie &stress, const Serie &positions) {
    Serie result(1, stress.count());

    for (uint32_t i = 0; i < stress.count(); ++i) {
        const Array &values = stress.get<Array>(i);
        const Array &pos = positions.get<Array>(i);

        double sigma1 = values[0];
        double sigma3 = values[2];
        double deviatoric = sigma1 - sigma3;

        double critical_stress = 2 * cohesion * std::cos(friction_angle) /
                                    (1 - std::sin(friction_angle));

        double depth_factor = std::exp(pos[2] / 1000.0);
        result.set(i, (deviatoric / critical_stress) * depth_factor);
    }
    return result;
};

// Input data structures
Serie stress   (6, {-2, 4, 6, -3, 6, -9, 1, 2, 3, 4, 5, 6, 9, 8, 7, 6, 5, 4});
Serie positions(3, {10, 20, -30, 1, 0, 0, 2, 0, 0});
Serie markers  (1, {1, 2, 2});

// Filter based on multiple conditions
auto filtered = df::filter(
    [](const Array &s, const Array &p, const Array &m) {
        return s[0] < 0 && // compressive stress
               p[2] < 0 && // depth condition
               m[0] == 1;  // specific rock type
    },
    stress, positions, markers
);

// Access filtered Series
auto filtered_stress    = filtered[0];
auto filtered_positions = filtered[1];
auto filtered_markers   = filtered[2];

// Use in pipeline
auto result = pipe(

    df::filter([](const Array &s, const Array &p) {
        return s[0] < 0 && p[2] < 0;
    }, stress, positions),

    [=](const df::Series &fs) {
        return computeCriticalityIndex(fs[0], fs[1]);
    }

);

std::cerr << "Result:" << std::endl << result << std::endl ;
```

## Licence
MIT

## Contact
fmaerten@gmail.com
