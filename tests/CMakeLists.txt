add_executable(demo "main.cpp")
set_target_properties(demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
target_link_options(demo PUBLIC -static -static-libgcc -static-libstdc++)
target_link_libraries(demo PRIVATE ogl_lib)
target_include_directories(
    demo
    PUBLIC
    # add additional dirs to add to include path
    "${CMAKE_BINARY_DIR}"
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB ALL_SHADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.*")
foreach(file ${ALL_SHADERS})
    message("Adding shader ${file}")
    file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/shaders/${file}" INPUT "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${file}")
endforeach()