add_library(ogl_lib STATIC
    model.cpp
    render_context.cpp
)

find_package(OpenGL REQUIRED)

target_include_directories(
    ogl_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    C://cxx_buildtools/Libs/glew-2.1.0/include
    C://cxx_buildtools/Libs/glfw-3.3.9/include
    C://cxx_buildtools/Libs/glm-1.0.1
)

## add glfw ##
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(C://cxx_buildtools/Libs/glfw-3.3.9/ ${CMAKE_BINARY_DIR}/Lib/glfw-3.3.9)

## add glew ##
add_subdirectory(C://cxx_buildtools/Libs/glew-2.1.0/build/cmake ${CMAKE_BINARY_DIR}/Lib/glew-2.1.0)

## add glm ##
set(DGLM_BUILD_TESTS OFF)
set(DGLM_DBUILD_SHARED_LIBS OFF)
add_subdirectory(C://cxx_buildtools/Libs/glm-1.0.1/ ${CMAKE_BINARY_DIR}/Lib/glm-1.0.1)

## add freetype ##
add_subdirectory(C://cxx_buildtools/Libs/freetype-2.13.3 ${CMAKE_BINARY_DIR}/Lib/freetype-2.13.3)

add_dependencies(ogl_lib glfw glew)
target_link_libraries(ogl_lib ${OPENGL_LIBRARY} glfw glew freetype)
