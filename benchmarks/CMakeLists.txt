include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(benchmarks "benchmark.cpp")
set_target_properties(benchmarks PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_sources(benchmarks PUBLIC
    utility.cpp
)
target_link_libraries(benchmarks PRIVATE ogl_lib)


# Copy resources
# file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB ALL_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*")
foreach(file ${ALL_SHADERS})
    message("Adding shader ${file}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/shaders/${file}" INPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${file}")
endforeach()