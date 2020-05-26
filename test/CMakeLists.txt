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
foreach (CompilerFlag ${CompilerFlags})
    string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
endforeach ()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
        EngineTests/ScriptingTests.hpp
        EngineTests/BehaviourTests.hpp
        ServerTests/UriTests.hpp
        ServerTests/ODBCTests.hpp
        ServerTests/ServerClientTests.hpp
        ServerTests/SsoTests.hpp)
set(Sources
        main_test.cpp)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
find_package(ODBC REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../engine/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../ ${OPENSSL_INCLUDE_DIR} ${ODBC_INCLUDE_DIR})
add_executable(${This} ${Sources} ${Headers})
target_link_libraries(${This} PUBLIC ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} glfw ${ODBC_LIBRARY})

IF (WIN32)
    find_package(glm REQUIRED)
    find_package(duktape REQUIRED)
    find_package(GTest CONFIG REQUIRED)
    target_link_libraries(${This} PUBLIC engine GTest::gtest GTest::gmock)
    target_link_libraries(${This} PUBLIC engine ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm ${OPENGL_INCLUDE_DIRS} ${DUKTAPE_LIBRARY})
ELSE ()
    target_link_libraries(${This} PUBLIC engine gtest gmock)
    target_link_libraries(${This} PUBLIC engine ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw /usr/lib/libduktape.so)
ENDIF ()

add_test(
        NAME ${This}
        COMMAND ${This}
)
