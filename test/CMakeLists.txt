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

set(Sources
    ExampleTests.cpp
        )

add_executable(${This} ${Sources})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(${This} PUBLIC
    gtest_main
    GameProject
    server
)

add_test(
    NAME ${This}
    COMMAND ${This}
)