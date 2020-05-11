cmake_minimum_required(VERSION 3.13.4)

set(This ExampleTests)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

set(Headers
        EngineTests/BufferTests.hpp)

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

foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()