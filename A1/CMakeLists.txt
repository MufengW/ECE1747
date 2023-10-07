cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(A1 VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files to the executable
add_executable(A1
    src/A1.cpp
    src/helper.cpp
)

