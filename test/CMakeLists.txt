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
string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
        EngineTests/ScriptingTests.hpp)
set(Sources
        main_test.cpp
)


include_directories( ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src )
add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} PUBLIC)

IF (WIN32)
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${This} PUBLIC GTest::gtest GTest::gmock)
ELSE()
    target_link_libraries(${This} PUBLIC gtest gmock)
ENDIF()

add_test(
    NAME ${This}
    COMMAND ${This}
)