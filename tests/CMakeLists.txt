## TEST SUITE ##
set(UTS_ROOT ${CMAKE_SOURCE_DIR}/../UnitTestSuite)
set(UTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${UTS_ROOT} ${CMAKE_BINARY_DIR}/Lib/testsuite)


include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_link_options(-static -static-libgcc -static-libstdc++)

# Demo
add_executable(demo "main.cpp")
set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(demo PRIVATE ogl_lib)

# Unit tests
add_executable(tests "tests.cpp")
set_target_properties(tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(tests PRIVATE ogl_lib testsuite)
target_include_directories(tests PUBLIC
    ${UTS_ROOT}/include
)


# Copy resources
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB ALL_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*")
foreach(file ${ALL_SHADERS})
    message("Adding shader ${file}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/shaders/${file}" INPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${file}")
endforeach()