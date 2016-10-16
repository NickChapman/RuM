cmake_minimum_required(VERSION 3.6)
project(Project2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp RuM.cpp RuM.h Token.cpp Token.h)
add_executable(Project2 ${SOURCE_FILES})