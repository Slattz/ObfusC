cmake_minimum_required(VERSION 3.13.4)
project(ObfusC)

# Set this to a valid LLVM installation dir
set(LT_LLVM_INSTALL_DIR "" CACHE PATH "/usr/bin/")

# Add the location of LLVMConfig.cmake to CMake search paths (so that
# find_package can locate it)
list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/llvm-15/cmake")

find_package(LLVM 15 REQUIRED CONFIG)

# Add the LLVM include paths and my includes
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})
include_directories("include")

# Use the same C++ standard as LLVM does
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

# LLVM is normally built without RTTI. Be consistent with that.
if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# Add Target and source dir 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../lib)
add_library(ObfusC SHARED main.cpp)
add_subdirectory("source")
