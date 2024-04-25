cmake_minimum_required(VERSION 3.12)
project(thread_server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(thread_server main_2.cpp)

set(CMAKE_CXX_STANDARD 20)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)