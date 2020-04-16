cmake_minimum_required(VERSION 3.13.4)

set(This Tests)

set(CompilerFlags
CMAKE_CXX_FLAGS
CMAKE_CXX_FLAGS_DEBUG
CMAKE_CXX_FLAGS_RELEASE
CMAKE_C_FLAGS
CMAKE_C_FLAGS_DEBUG
CMAKE_C_FLAGS_RELEASE
)
foreach(CompilerFlag ${CompilerFlags})
string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
        ServerTests/UriTests.hpp)
set(Sources
        main_test.cpp
)

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