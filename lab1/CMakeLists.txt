cmake_minimum_required(VERSION 3.16)
project(task1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(USE_DOUBLE "Use double" OFF)

add_executable(task1 task1.cpp)

if(USE_DOUBLE)
   add_definitions(-DUSE_DOUBLE)
endif()