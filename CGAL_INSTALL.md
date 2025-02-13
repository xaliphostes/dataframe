# CGAL activation
By default, CGAL is not activated.

If you plan to use it and the associated functions, you need to activate it with the flag `USE_CGAL`: 
```bash
cmake -DUSE_CGAL=ON ..
```

and all test files located in `unitTest/cgal` will be compiled and launched.

But before generating the Makefiles with cmake, you need to install CGAL (see below).

# CGAL installation

To install CGAL and have it recognized by CMake, you have a few options:

## Using Package Manager (easiest):
### For Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libcgal-dev
```
### For macOS (using Homebrew):
```bash
brew install cgal
```
### For Windows (using vcpkg):
```bash
vcpkg install cgal:x64-windows
```

## Building from source:
```bash
# Download and extract CGAL from https://github.com/CGAL/cgal/releases
wget https://github.com/CGAL/cgal/releases/download/v5.6/CGAL-5.6.tar.xz
tar xf CGAL-5.6.tar.xz
cd CGAL-5.6
cmake .
make
sudo make install
```
After installation, you might need to tell CMake where to find CGAL if it's not in a standard location. You can do this by setting CMAKE_PREFIX_PATH when running cmake:
```bash
cmake -DUSE_CGAL=ON -DCMAKE_PREFIX_PATH=/path/to/cgal/installation ..
```
CGAL also has some dependencies that need to be installed:
### For Ubuntu/Debian:
```bash
sudo apt-get install libgmp-dev libmpfr-dev
```
### For macOS:
```bash
brew install gmp mpfr
```
### For Windows:
`vcpkg` will handle the dependencies automatically.