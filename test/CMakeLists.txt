cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)

set(Headers
        EngineTests/BufferTests.hpp)

set(Sources
    ExampleTests.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    gtest_main
    GameProject
        engine
)

add_test(
    NAME ${This}
    COMMAND ${This}
)