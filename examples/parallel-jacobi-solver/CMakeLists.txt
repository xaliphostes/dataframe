project(parallel-jacobi-solver)

add_executable(${PROJECT_NAME} main.cxx)

target_link_libraries(${PROJECT_NAME}
    dataframe-shared
)

set_target_properties(parallel-jacobi-solver PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
