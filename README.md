# Dataframe

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=soon&color=red" alt="Windows support">
  <img src="https://img.shields.io/static/v1?label=Linux&logo=linux&logoColor=white&message=support&color=success" alt="Linux support">
  <img src="https://img.shields.io/static/v1?label=macOS&logo=apple&logoColor=white&message=support&color=success" alt="macOS support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  
</p>

Minimalist [Panda](https://pandas.pydata.org/)-like library in C++ which allows you to create and manage series and dataframes. It also provides math functions to play with Series.

Compared to Panda, we have some differences since our main consern is **linear algebra**.

Main functionalities are:
- Compared to Panda, each item of a Serie is either a scalar (i.e., a number) or an array of scalars... and that's it!. This is it because our main concern is linear algebra.
- `Serie` of scalar items or n-dim items
- Mathematical functions on `Serie`
- Decomposition of a `Serie` into a new `Serie` (virtual Serie) using the `Manager` and some `Decomposer`s
- Functional programming (no mutating, less bugs, very simple to use)
- Very simple API for both `Serie` and `Dataframe`
- Can be easily extended for mathematical functions such as **eigenValues** or **eigenVectors** on a `Serie`

Not yet tested under Windows, but will have to add `export` for shared library.

# Organization of this lib

- Main files are `Dataframe` and `Serie`
- Folder `math` provides some examples of what is possible to do with `Serie` (see also the [dataframe in TypeScript](https://github.com/youwol/dataframe))
- Folder `attributes` provides a way of decomposing any `Serie` into other `Serie`. For example, a `Serie` with `itemType=6` might represent symmetric matrices 3x3. Therefore, attributes (i.e., possible decomposed `Serie`s) can be :
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
In the same directory, type
```sh
ctest
```

# API

## Example 1
```c++
df::Serie a(3, {1,2,5,  3,4,9}) ; // first param is the dim of the Serie: 3
df::Serie b(3, {4,3,3,  2,1,0}) ;

df::Serie dot = df::dot(a, b) ;
dot.dump();
```

## Example 2
Performs a weighted sum of Series ;-)

Constraints:
- All `Serie` must have the same `count()`
- All `Serie` must have the same `itemSize()`
- Size of weights must be equal to the size of array of the provided `Serie`s
```c++
df::Serie a(2, {1,2,  3,4}) ;
df::Serie b(2, {4,3,  2,1}) ;
df::Serie c(2, {2,2,  1,1}) ;

auto s = df::weigthedSum({a, b, c}, {2, 3, 4}) ;
```

## Example 3
Possible extension (very simple to implement) ;-)
```c++
df::Serie s(6, {....}) ; // symmetric 3x3 matrices => 6 coefs
auto vectors = df::eigenVectors(s) ; // and that's it!

// Display
vectors.forEach([](const Array& v, uint32_t index) {
    std::cout << "eigen vector at index " 
              << std::to_string(index)
              << " is: " << v  << std::endl ;
});
```

## Example 4: Chaining...
```c++
df::Serie a(2, {1,2,  3,4}) ;
df::Serie b(2, {4,3,  2,1}) ;
df::Serie c(2, {2,2,  1,1}) ;

auto s = df::weigthedSum({a, b, c}, {2, 3, 4})
    .map([](const Array& arr, uint32_t) {
        return std::sqrt( pow(arr[0],2) + pow(arr[1],2) + pow(arr[2],2) );
    })
    .forEachScalar([](double v, uinit32_t index) {
        std::cout << index << ": " << v << std::endl ;
    }) ;
```

## Example 5: Attributes
```c++
df::Manager mng(dataframe, {
    PositionDecomposer(),
    ComponentDecomposer(),
    EigenValuesDecomposer(),
    EigenVectorsDecomposer()
])

// Gather possible scalar series names
console.log( mng.names(1) )
// Will display: Sxx, Sxy, Sxz, Syy, Syz, Szz, S1, S2, S

Serie scalarS1 = mng.serie(1, 'S1') // eigen values for S1
Serie vectorS1 = mng.serie(3, 'S1') // eigen vectors for S1
```

## Licence
MIT

## Contact
fmaerten@gmail.com
