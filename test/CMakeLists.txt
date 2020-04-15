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
foreach(CompilerFlag ${CompilerFlags})
string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
        )
set(Sources
        main_test.cpp
)

add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} PUBLIC
    gtest_main
    GameProject
)

add_test(
    NAME ${This}
    COMMAND ${This}
)