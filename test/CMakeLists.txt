cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(Headers
        EngineTests/BufferTests.hpp
        EngineTests/ScriptingTests.hpp)

set(Sources
        ExampleTests.cpp)

add_executable(${This} ${Sources} ${Headers})
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src
        ${GLEW_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
        ${GLM_INCLUDE_DIR}
)
target_link_libraries(${This} PUBLIC gtest_main ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)

add_test(
    NAME ${This}
    COMMAND ${This}
)