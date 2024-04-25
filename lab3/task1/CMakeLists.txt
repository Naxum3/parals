cmake_minimum_required(VERSION 3.12)
project(task1 VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenMP)

include(CTest)
enable_testing()

add_executable(task1 main_1.cpp)

set (CMAKE_CXX_STANDARD 20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)