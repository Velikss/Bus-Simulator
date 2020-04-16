cmake_minimum_required(VERSION 3.13.4)

set(This GameProject)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)
add_subdirectory(engine)
add_subdirectory(engine/glfw)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
)
set(Sources 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/src ${GLUT_INCLUDE_DIRS})
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})

add_subdirectory(test)