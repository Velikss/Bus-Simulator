cmake_minimum_required(VERSION 3.13.4)

set(This engine)
project(${This} C CXX)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
)
set(Sources 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})
target_link_libraries(${This}-bin ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES})
