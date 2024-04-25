cmake_minimum_required(VERSION 3.0.0)
project(task3 VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenMP)

include(CTest)
enable_testing()

add_executable(task3 main_3.cpp)

#target_link_libraries(task3 PRIVATE m)

target_link_libraries(task3 PRIVATE "${OpenMP_C_FLAGS}")
target_compile_options(task3 PRIVATE "${OpenMP_C_FLAGS}")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)