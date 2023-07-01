# Purpose
The purpose of this sub-library is to deal with virtual Series that can be created from various operations such as matrix components, eigen vectors and values and so on.

The main class to use is the `Manager` which deals with possible `Serie`s according to the installed `Decomposer`(s).

# Example
Here is a simple example
```c++
df::Dataframe dataframe;
dataframe.add("positions", df::Serie(3, {2,4,6, 3,6,9, 1,2,3})); // vector 3
dataframe.add("indices", df::Serie(3, {0,1,2})); // vector 3
dataframe.add("S", df::Serie(6, {2,4,6,3,6,9, 1,2,3,4,5,6, 9,8,7,6,5,4})); // sym matrix 3x3

df::Manager mng(dataframe, {
    new df::Coordinates(), // x,y,z if applicable
    new df::Components(),  // vectors and matrices are decomposed
    new df::EigenValues(), // eigen values if applicable
    new df::EigenVectors(),// eigen vectors if applicable
    new df::Normals(),     // normal vectors if applicable
    new df::Area()         // polygon areas if applicable
}, 3); // 3 stands for 3-dimensional

std::cerr << "Available series of scalars :\n" << mng.names(1) << std::endl;
std::cerr << "Available series of vector3 :\n" << mng.names(3) << std::endl;
std::cerr << "Available series of matrix33:\n" << mng.names(6) << std::endl;
```

Will display

```sh
Available series of scalars :
  S1
  S2
  S3
  Sxx
  Sxy
  Sxz
  Syy
  Syz
  Szz
  area
  x
  y
  z

Available series of vector3 :
  S1
  S2
  S3
  normals

Available series of matrix33:
  S
```
