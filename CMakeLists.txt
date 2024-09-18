cmake_minimum_required(VERSION 3.24)
project(cplusplus_efficiency)

set(CMAKE_CXX_STANDARD 17)

# Check if the current build type is not set, default to Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add the executable
add_executable(cplusplus_efficiency main.cpp)

# Set compile options for -O0 (no optimization)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
