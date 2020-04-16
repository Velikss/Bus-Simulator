cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)

set(Headers
        EngineTests/BufferTests.hpp)

set(Sources
        ExampleTests.cpp)

add_executable(${This} ${Sources} ${Headers})
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}/../engine/glfw/include ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src)
target_link_libraries(${This} PUBLIC gtest_main)

add_test(
    NAME ${This}
    COMMAND ${This}
)