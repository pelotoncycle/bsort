# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

project(bsort)

# We build using c++11
set(CMAKE_CXX_STANDARD 11)

# Set optimization flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# We use OpenMP for parallelism
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -fopenmp")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

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
