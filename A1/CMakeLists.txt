cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(A1 VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add option for MPI
option(USE_MPI "Use MPI for parallelization" OFF)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add strict compiler flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    endif()
endif()

# Add source files to the executable
add_executable(A1
    src/A1.cpp
    src/data_processor.cpp
    src/parallel.cpp
    src/computation.cpp
    src/log.cpp
)

# Find required packages
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(A1 PRIVATE Threads::Threads)

# Conditionally find and link MPI
message(STATUS "USE_MPI is set to ${USE_MPI}")

if(USE_MPI)
    find_package(MPI REQUIRED)
    target_link_libraries(A1 PRIVATE ${MPI_LIBRARIES})
    include_directories(${MPI_INCLUDE_PATH})
    if(MPI_COMPILE_FLAGS)
        set_target_properties(A1 PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()
    if(MPI_LINK_FLAGS)
        set_target_properties(A1 PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()
    add_definitions(-DUSE_MPI)
endif()
