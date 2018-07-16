# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

# We build using c++11
set(CMAKE_CXX_STANDARD 11)

# Project's name
project(bsort)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# build a static library
add_library(bsort STATIC ${CMAKE_SOURCE_DIR}/src/bsort.cpp)

add_executable(bsort-main ${CMAKE_SOURCE_DIR}/src/main.cpp )
target_link_libraries(bsort-main "${LIBRARY_OUTPUT_PATH}/libbsort.a")
set_target_properties(bsort-main PROPERTIES OUTPUT_NAME "bsort")
add_dependencies(bsort-main bsort)
