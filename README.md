# Dataframe

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

# Organization of this lib

- Main files are `Dataframe` and `Serie`
- Folder `math` provides some examples of what is possible to do with `Serie` (see also the [dataframe in TypeScript](https://github.com/youwol/dataframe))

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

vectors.forEach([](const Array& v, uint32_t index) {
    std::cout << "eigen vector at index " 
              << std::to_string(index)
              << " is: " << v  << std::endl ;
});
```

## Licence
MIT

## Contact
fmaerten@gmail.com
