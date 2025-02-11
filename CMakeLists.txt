# Copyright (c) 2024-now fmaerten@gmail.com
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
cmake_minimum_required(VERSION 3.5)
project(dataframe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS on)

include(FetchContent)

# Eigen library for linear algebra operations (!optional)
set(BUILD_TESTING OFF CACHE BOOL "Disable Eigen tests" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "Disable Eigen documentation" FORCE)
set(EIGEN_BUILD_TESTS OFF CACHE BOOL "Disable Eigen tests" FORCE)
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)
# target_link_libraries(dataframe PRIVATE Eigen3::Eigen)

# ---------------------------------------------------------------------
# Detect all testing files
# ---------------------------------------------------------------------
function (auto_discover_tests)
    file(GLOB_RECURSE TEST_FILES "*_test.cxx")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE})
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endfunction()

# ---------------------------------------------------------------------
# Example of configuration:
#   cmake -DCMAKE_BUILD_TYPE=Release ..
# ---------------------------------------------------------------------
message("Setting build type to ${CMAKE_BUILD_TYPE}")

include_directories(include)
include_directories(${eigen_SOURCE_DIR})
message("Eigen3 include directory: ${eigen_SOURCE_DIR}")

enable_testing()
auto_discover_tests()

add_subdirectory(examples/complex)
add_subdirectory(examples/excel)
