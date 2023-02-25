# dataframe

Minimalist [Panda](https://pandas.pydata.org/)-like data structure in C++ which allows to create and manage series and dataframes.

Not tested yet under Windows, but will have to add `export` for shared library.

## Example
```c++
Dataframe dataframe ;

Array array = {0,1,3,2,7,8,7,6, 9,7,4,3,2,8,5,5} ;
dataframe.add("positions", Serie(8, array)) ;

auto& positions = dataframe["positions"] ;
positions.forEach( [](Array v, uint32_t i) {
    std::cerr << "[" << i << "]: " << v << std::endl ;
}) ;
```
