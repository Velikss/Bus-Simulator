cmake_minimum_required(VERSION 3.13.4)

set(This engine)
project(${This} C CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

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
        ${PROJECT_SOURCE_DIR}/src/Engine.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/Window.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/VulkanInstance.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/PhysicalDevice.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/LogicalDevice.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/SwapChain.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/GraphicsPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/RenderHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/uniform/UniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/ImageHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/CommandHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/Geometry.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/Vertex.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/BufferHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/ModelHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/mesh/Mesh.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/texture/Texture.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/texture/TextureHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/texture/TextureInfo.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/CommandBuffer.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/CommandBufferRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/IndexedRenderRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/ClearScreenRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/Scene.hpp src/vulkan/scene/BaseObject.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/TestScene.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/InputHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/StreetScene.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/TextTest.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/OverlayPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/PipelineHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/Vertex2D.hpp src/vulkan/overlay/OverlayRenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/OverlayCommandBufferRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/Font.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/uniform/UniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/OverlayUniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/overlay/Text.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/renderpass/RenderPass.hpp)
set(Sources
        ${PROJECT_SOURCE_DIR}/src/main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})
target_link_libraries(${This}-bin ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw ${Vulkan_LIBRARIES})

IF (NOT WIN32)
    add_custom_command(TARGET ${This}-bin PRE_BUILD COMMAND cd ${PROJECT_SOURCE_DIR}/shaders && ./compile.sh)
ELSE ()
    add_custom_command(TARGET ${This}-bin PRE_BUILD COMMAND cd ${PROJECT_SOURCE_DIR}/shaders && ./compile.bat)
ENDIF ()
