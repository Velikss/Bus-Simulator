cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)

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

set(Headers
        ${PROJECT_SOURCE_DIR}/src/pch.hpp
        Server-Tests/ServerHttpTests.hpp)
set(Sources
        test.cpp
        Server-Tests/ServerHttpTests.hpp)

add_executable(${This} ${Sources} ${Headers})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${This} PUBLIC
    gtest_main
    GameProject
    server
)

add_test(
    NAME ${This}
    COMMAND ${This}
)