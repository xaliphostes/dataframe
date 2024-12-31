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

set(DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# ---------------------------------------------------------------------
#              T E S T I N G
# ---------------------------------------------------------------------
function (auto_discover_tests)
    file(GLOB_RECURSE TEST_FILES "*_test.cxx")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_FILE})
        target_link_libraries(${TEST_NAME} PRIVATE dataframe)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endfunction()
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
#              D O C U M E N T A T I O N
# To automatically build documentation with your project:
#     add_dependencies(your_target docs)
#
# To generate the documentation, build the docs target:
#     cmake -B build
#     cmake --build build --target docs
# ---------------------------------------------------------------------
# Find Doxygen package
find_package(Doxygen)

if(DOXYGEN_FOUND)
    # Set Doxygen configuration variables
    set(DOXYGEN_INPUT_DIR ${CMAKE_SOURCE_DIR}/include)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)
    set(DOXYFILE_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE ${CMAKE_BINARY_DIR}/Doxyfile)

    # Configure the Doxyfile
    configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

    # Add custom target for documentation
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM)
else()
    message(STATUS "Doxygen not found, documentation will not be generated")
endif()
# ---------------------------------------------------------------------

# ---------------------------------------------------------------------
#              C O V E R A G E
# ---------------------------------------------------------------------
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
# ---------------------------------------------------------------------

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

file(GLOB_RECURSE SRC "${PROJECT_SOURCE_DIR}/include/geophysics/*.h" "${PROJECT_SOURCE_DIR}/src/*.cxx")
add_library(dataframe SHARED ${SRC})

set_target_properties(dataframe PROPERTIES OUTPUT_NAME Dataframe)

set_target_properties(dataframe PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
set_target_properties(dataframe PROPERTIES PDB_OUTPUT_DIRECTORY ${DIST_DIR})

# examples
add_subdirectory(examples)

# Testing...
enable_testing()
auto_discover_tests()
