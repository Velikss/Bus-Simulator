cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)

set(Sources
    ExampleTests.cpp
)

add_executable(${This} ${Sources})
target_link_libraries(${This} PUBLIC
    gtest_main
    GameProject
    server
)

add_test(
    NAME ${This}
    COMMAND ${This}
)