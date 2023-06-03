# dataframe

Minimalist [Panda](https://pandas.pydata.org/)-like library in C++ which allows you to create and manage series and dataframes. It also provides math functions to play with Series.

Main functionalities are:
- Compared to Panda, each item of a Serie is either a scalar (i.e., a number) or an array of scalars... and that's it!. This is it because our main concern is linear algebra.
- `Serie` of scalar items or n-dim items
- Mathematical functions on `Serie`
- Decomposition of a `Serie` into a new `Serie` (virtual Serie) using the `Manager` and some `Decomposer`s
- Functional programming (no mutating, less bugs, very simple to use)
- Very simple API
- Can be extended for mathematical functions such as **eigenValues** or **eigenVectors** on a `Serie`

Not tested yet under Windows, but will have to add `export` for shared library.

## Example 1
```c++
df::Serie a(2, Array({1,2,  3,4})) ;
df::Serie b(2, Array({4,3,  2,1})) ;

Serie s = df::dot(a, b) ;
```

## Example 2
Performs a weighted sum of Series ;-)

Constraints:
- All `Serie` must have the same `count()`
- All `Serie` must have the same `itemSize()`
- Size of `weights` must be equal to the size of array of the provided `Serie`s
```c++
df::Serie a(2, Array({1,2,  3,4})) ;
df::Serie b(2, Array({4,3,  2,1})) ;
df::Serie c(2, Array({2,2,  1,1})) ;

Array weights{2, 3, 4} ;

auto s = df::weigthedSum({a, b, c}, weights) ;
```

## Example 3
Possible extension (very simple to implement) ;-)
```c++
df::Serie s(6, Array(....)) ; // symmetric 3x3 matrices
auto vectors = df::eigenVectors(s) ; // and that's it!

vectors.forEach([](const Array& v, uint32_t index) {
    std::cout << "eigen vector at index " 
              << std::to_string(index)
              << " is " << v  << std::endl ;