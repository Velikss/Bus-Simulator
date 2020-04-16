cmake_minimum_required(VERSION 3.13.4)

set(This server)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
        src/Uri.hpp)
set(Sources 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/src )
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})