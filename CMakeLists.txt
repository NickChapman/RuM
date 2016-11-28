cmake_minimum_required(VERSION 3.6)
project(RuM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp RuM.cpp RuM.h Token.cpp Token.h RuMParser.cpp RuMParser.h Type.h Scope.cpp Scope.h)
add_executable(rum ${SOURCE_FILES})
