cmake_minimum_required(VERSION 3.13.4)

set(This engine)
project(${This} C CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#add_subdirectory(glfw)

set(Headers 
    ${PROJECT_SOURCE_DIR}/src/pch.hpp
    ${PROJECT_SOURCE_DIR}/src/shaders/glsl.hpp
    ${PROJECT_SOURCE_DIR}/src/shaders/shader.hpp
    ${PROJECT_SOURCE_DIR}/src/shaders/ShaderProgram.hpp
    ${PROJECT_SOURCE_DIR}/src/objects/BaseObject.hpp
    ${PROJECT_SOURCE_DIR}/src/objects/Light.hpp
    ${PROJECT_SOURCE_DIR}/src/objects/Material.hpp
    ${PROJECT_SOURCE_DIR}/src/objects/Mesh.hpp
    ${PROJECT_SOURCE_DIR}/src/objects/ObjectGroup.hpp
    ${PROJECT_SOURCE_DIR}/src/buffers/Buffer.hpp
    ${PROJECT_SOURCE_DIR}/src/geometries/Geometry.hpp
    ${PROJECT_SOURCE_DIR}/src/geometries/SkyBoxGeometry.hpp
    ${PROJECT_SOURCE_DIR}/src/vendor/tiny_obj_loader.h
    ${PROJECT_SOURCE_DIR}/src/loaders/fileloader.hpp
    ${PROJECT_SOURCE_DIR}/src/loaders/objectloader.hpp
    ${PROJECT_SOURCE_DIR}/src/vendor/stb_image.h
    ${PROJECT_SOURCE_DIR}/src/textures/Texture.hpp
    ${PROJECT_SOURCE_DIR}/src/textures/CubeMapTexture.hpp
    ${PROJECT_SOURCE_DIR}/src/scenes/Scene.hpp
    ${PROJECT_SOURCE_DIR}/src/scenes/Camera.hpp
    ${PROJECT_SOURCE_DIR}/src/scenes/MyStreetScene.hpp
    ${PROJECT_SOURCE_DIR}/src/animations/Animation.hpp
    ${PROJECT_SOURCE_DIR}/src/animations/FireWehrAnimation.hpp
    ${PROJECT_SOURCE_DIR}/src/animations/LightObjectBindingAnimation.hpp
    ${PROJECT_SOURCE_DIR}/src/animations/MoonLightAnimation.hpp
    ${PROJECT_SOURCE_DIR}/src/animations/OrbitalAnimation.hpp
    ${PROJECT_SOURCE_DIR}/src/scenes/TestScene.hpp
)
set(Sources 
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

include_directories(${PROJECT_SOURCE_DIR}/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})
target_link_libraries(${This}-bin ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw)
