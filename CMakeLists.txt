cmake_minimum_required(VERSION 3.15.5)

set(This GameProject)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)

set(Headers ${PROJECT_SOURCE_DIR}/src/pch.hpp)
set(Sources ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_library(${This} STATIC ${Sources} ${Headers})

add_subdirectory(test)