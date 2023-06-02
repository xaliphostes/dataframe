# dataframe

Minimalist [Panda](https://pandas.pydata.org/)-like library in C++ which allows you to create and manage series and dataframes. It also provides math functions to play with Series.

Main functionalities are:
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
```c++
df::Serie a(2, Array({1,2,  3,4})) ;
df::Serie b(2, Array({4,3,  2,1})) ;
df::Serie c(2, Array({2,2,  1,1})) ;

Array alpha{2, 3, 4} ;

auto s = df::weigthedSum(df::Series({a, b, c}), alpha) ;
```
