cmake_minimum_required(VERSION 2.8)
project(tsanlog-pur)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -Wextra")

set(SOURCE_FILES main.cpp)

add_executable(tsanlog-pur main.cpp)