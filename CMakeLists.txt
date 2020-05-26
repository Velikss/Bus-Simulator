cmake_minimum_required(VERSION 3.13.4)

set(This Bus-Simulator)
set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(server)
add_subdirectory(engine)

set(Headers
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
)
set(Sources
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src ${ENGINE_INCLUDE_DIRS})
add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} ${ENGINE_LIBRARIES})

add_subdirectory(test)