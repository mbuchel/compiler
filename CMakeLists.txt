cmake_minimum_required(VERSION 3.5)
project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp VM.cpp VM.h)
add_executable(compiler ${SOURCE_FILES})