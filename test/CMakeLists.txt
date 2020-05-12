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
        EngineTests/BehaviourTests.hpp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

include_directories( ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} )
add_executable(${This} ${Sources} ${Headers})

IF (WIN32)
    find_package(glm REQUIRED)
    find_package(duktape REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${This} PUBLIC engine GTest::gtest GTest::gmock)
    target_link_libraries(${This} PUBLIC engine ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm ${OPENGL_INCLUDE_DIRS} ${DUKTAPE_LIBRARY})
ELSE()
    target_link_libraries(${This} PUBLIC engine gtest gmock)
    target_link_libraries(${This} PUBLIC engine ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw /usr/lib/libduktape.so)
ENDIF()

add_test(
    NAME ${This}
    COMMAND ${This}
)