cmake_minimum_required(VERSION 3.13.4)

set(This Bus-Simulator)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

enable_testing()

add_subdirectory(server)
add_subdirectory(engine)

set(Headers
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
)
set(Sources
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} engine)

IF (WIN32)
    add_custom_command(TARGET ${This}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/engine/vendor/fmod/windows/api/core/lib/x64/fmod.dll"
            $<TARGET_FILE_DIR:${This}>)
ENDIF()