cmake_minimum_required(VERSION 3.13.4)

set(This GameProject)
project(${This} C CXX)

IF ( CMAKE_COMPILER_IS_MSVC )
    SET (
            CMAKE_CXX_FLAGS_DEBUG
            "${CMAKE_CXX_FLAGS_DEBUG} /MTd"
    )
    SET (
            CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} /MT"
    )
ENDIF ( CMAKE_COMPILER_IS_MSVC )

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
)
set(Sources 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/src )
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})

add_subdirectory(test)