cmake_minimum_required(VERSION 3.13.4)

set(This server)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
        ${PROJECT_SOURCE_DIR}/src/Uri.hpp
        ${PROJECT_SOURCE_DIR}/src/DirectoryWatcher.hpp
        vendor/Json.hpp
        vendor/Utf8.hpp src/NetworkAbstractions.hpp src/SslHelper.hpp src/ODBC/ODBCInstance.hpp vendor/StdUuid.hpp src/SSO/SsoServer.hpp src/SSO/SsoService.hpp src/GameServer/GameServer.hpp src/SSO/SsoClient.hpp src/SSO/SsoHelper.hpp src/GameServer/GameConnectionHelper.hpp)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ODBC REQUIRED)

include_directories( ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ ${PROJECT_SOURCE_DIR}/src ${OPENSSL_INCLUDE_DIR} ${ODBC_INCLUDE_DIR})
add_executable(${This}-Test ${PROJECT_SOURCE_DIR}/test/main.cpp ${Headers})
add_executable(${This} ${PROJECT_SOURCE_DIR}/src/main.cpp ${Headers})

IF (WIN32)
    find_package(glm REQUIRED)
    target_link_libraries(${This} PUBLIC OpenSSL::SSL OpenSSL::Crypto ${ODBC_LIBRARY} glm)
    target_link_libraries(${This}-Test PUBLIC OpenSSL::SSL OpenSSL::Crypto ${ODBC_LIBRARY} glm GTest::gtest)
ELSE()
    target_link_libraries(${This} PUBLIC ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ODBC_LIBRARY})
    target_link_libraries(${This}-Test PUBLIC ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ODBC_LIBRARY} gtest)
ENDIF()

add_test(
        NAME ${This}-Test
        COMMAND ${This}-Test
)