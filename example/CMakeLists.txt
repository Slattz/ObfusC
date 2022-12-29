cmake_minimum_required(VERSION 3.13.4)
project(ObfusCExample)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpass-plugin=${CMAKE_BINARY_DIR}/../../lib/libObfusC.so")

# Add Target and source dir 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
add_executable(ObfusCExample main.cpp)
