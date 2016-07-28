cmake_minimum_required(VERSION 3.5)
project(compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Parser.cpp Parser.h Processor.cpp Processor.h Error.cpp Error.h lex.h lex.cpp)
add_executable(compiler ${SOURCE_FILES})