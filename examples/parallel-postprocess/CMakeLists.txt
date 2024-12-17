project(parallel-postprocess)

add_executable(${PROJECT_NAME} main.cxx)

target_link_libraries(${PROJECT_NAME}
    dataframe
)

set_target_properties(parallel-postprocess PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${DIST_DIR})
