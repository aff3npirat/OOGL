cmake_minimum_required(VERSION 3.10)
project(oglengine VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(source)

configure_file(cmake_config.h.in "${CMAKE_BINARY_DIR}/cmake_config.h")