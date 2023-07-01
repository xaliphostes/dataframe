# Copyright (c) 2023 fmaerten@gmail.com
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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS on)

set(DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if(ENABLE_COVERAGE)
  # set compiler flags
  set(CMAKE_CXX_FLAGS "-O0 -coverage")

  # find required tools
  find_program(LCOV lcov REQUIRED)
  find_program(GENHTML genhtml REQUIRED)

  # add coverage target
  add_custom_target(coverage
    # gather data
    COMMAND ${LCOV} --directory . --capture --output-file coverage.info
    # generate report
    # --ignore-errors inconsistent 
    COMMAND ${GENHTML}--demangle-cpp -o coverage coverage.info
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

IF (WIN32)
    add_definitions( -DATAFRAME_EXPORTS=TRUE )
ENDIF()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
else()
    message("Setting build type to 'Debug' (specified in cli).")
endif()

include_directories(include)

set(SRC_dataframe
    ./src/dataframe/Dataframe.cxx
    ./src/dataframe/Serie.cxx
    ./src/dataframe/utils/nameOfSerie.cxx
    ./src/dataframe/attributes/Manager.cxx
    ./src/dataframe/attributes/Decomposer.cxx
    ./src/dataframe/attributes/Components.cxx
    ./src/dataframe/attributes/Normals.cxx
    ./src/dataframe/attributes/Coordinates.cxx
    ./src/dataframe/attributes/Valence.cxx
    ./src/dataframe/attributes/Area.cxx
    ./src/dataframe/math/add.cxx
    ./src/dataframe/math/dot.cxx
    ./src/dataframe/math/negate.cxx
    ./src/dataframe/math/scale.cxx
    ./src/dataframe/math/weightedSum.cxx
    ./src/dataframe/math/transpose.cxx
    ./src/dataframe/math/div.cxx
    ./src/dataframe/math/norm.cxx
)

add_library(dataframe-shared SHARED ${SRC_dataframe})
set_target_properties(dataframe-shared PROPERTIES OUTPUT_NAME Dataframe)

set_target_properties(dataframe-shared PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe-shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe-shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe-shared PROPERTIES PDB_OUTPUT_DIRECTORY ${DIST_DIR})

# Testing...
enable_testing()
add_subdirectory(unitTest)
