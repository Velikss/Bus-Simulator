cmake_minimum_required(VERSION 3.13.4)

set(This Bus-Simulator)
project(${This} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

enable_testing()

add_subdirectory(server)
add_subdirectory(engine)

set(Headers
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
        src/overlay/MainMenu.hpp engine/src/vulkan/module/overlay/element/elements/Button.hpp engine/src/vulkan/module/overlay/element/elements/LabeledElement.hpp src/scenes/BlankScene.hpp src/overlay/BaseMenu.hpp src/overlay/LoadingOverlay.hpp
        src/overlay/MissionMenu.hpp
        src/logic/TrafficLightController.hpp src/overlay/SettingsMenu.hpp src/overlay/InGame.hpp src/overlay/MessageBoxOverlay.hpp)
set(Sources
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/server/src ${PROJECT_SOURCE_DIR}/server/vendor)
add_executable(${This} ${Sources} ${Headers} src/overlay/TestOverlay.hpp)
target_link_libraries(${This} engine)

IF (WIN32)
    add_custom_command(TARGET ${This}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_SOURCE_DIR}/engine/vendor/fmod/windows/api/core/lib/x64/fmod.dll"
            $<TARGET_FILE_DIR:${This}>)
ENDIF()