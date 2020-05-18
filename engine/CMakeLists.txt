cmake_minimum_required(VERSION 3.13.4)

set(This engine)
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

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig)
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

#IF(NOT WIN32)
    find_package(Vulkan REQUIRED)
#ENDIF()

add_subdirectory(glfw)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

set(Headers
        ${PROJECT_SOURCE_DIR}/src/pch.hpp
        ${PROJECT_SOURCE_DIR}/src/vendor/tiny_obj_loader.h
        ${PROJECT_SOURCE_DIR}/src/loaders/fileloader.hpp
        ${PROJECT_SOURCE_DIR}/src/loaders/objectloader.hpp
        ${PROJECT_SOURCE_DIR}/src/vendor/stb_image.h
        ${PROJECT_SOURCE_DIR}/src/Engine.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/Window.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/VulkanInstance.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/PhysicalDevice.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/LogicalDevice.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/RenderHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/uniform/UniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/ImageHelper.hpp
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
        ${PROJECT_SOURCE_DIR}/src/vulkan/command/ClearScreenRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/Scene.hpp src/vulkan/scene/BaseObject.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/TestScene.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/InputHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/StreetScene.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/PipelineHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/uniform/UniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/renderpass/RenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/util/Formatter.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/RenderModule.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/RenderPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/swapchain/SwapChainHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/swapchain/SwapChain.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/Shaders.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/mrt/MRTPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/mrt/MRTRenderModule.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/mrt/MRTRenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/mrt/MRTUniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/mrt/MRTRenderRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/lighting/LightingPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/lighting/LightingRenderModule.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/lighting/LightingRenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/lighting/LightingUniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/lighting/LightingRenderRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayRenderModule.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayRenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayUniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayCommandBufferRecorder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/text/Vertex2D.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/text/Text.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/text/Font.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/loop/GameLoop.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/loop/TickTask.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/LightObject.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/EngineLog.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/ViewportQuadGeometry.hpp src/vulkan/texture/TextureSampler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/BusCamera.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/BusWorldScene.hpp
        ${PROJECT_SOURCE_DIR}/src/pch.hpp
        ${PROJECT_SOURCE_DIR}/src/vendor/tiny_obj_loader.h
        ${PROJECT_SOURCE_DIR}/src/loaders/fileloader.hpp
        ${PROJECT_SOURCE_DIR}/src/loaders/objectloader.hpp
        ${PROJECT_SOURCE_DIR}/src/vendor/stb_image.h
        ${PROJECT_SOURCE_DIR}/src/scripting/cScriptingEngine.hpp
        ${PROJECT_SOURCE_DIR}/src/scripting/functions/JavaScriptBaseFunctions.hpp
        ${PROJECT_SOURCE_DIR}/src/scripting/functions/JavaScriptEntityFunctions.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/cEntityInterface.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/cEntity.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/cEntityGroup.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/cBehaviourHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/multiplayer/SslHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/multiplayer/NetworkAbstractions.hpp
        ${PROJECT_SOURCE_DIR}/src/multiplayer/cNetworkConnection.hpp
        ${PROJECT_SOURCE_DIR}/src/multiplayer/cNetworkClient.hpp src/multiplayer/cMultiplayerHandler.hpp ${PROJECT_SOURCE_DIR}/src/vulkan/entities/cBus.hpp)
set(Sources
        ${PROJECT_SOURCE_DIR}/src/main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/src ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS} ${DUKTAPE_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
add_library(${This} STATIC ${Sources} ${Headers})
add_executable(${This}-bin ${Sources} ${Headers})

IF (NOT WIN32)
    target_link_libraries(${This} PUBLIC /usr/lib/libuuid.a)
    add_custom_command(TARGET ${This}-bin PRE_BUILD COMMAND cd ${PROJECT_SOURCE_DIR}/shaders && ./compile.sh)
ENDIF ()

IF (WIN32)
    find_package(glm REQUIRED)
    find_package(duktape REQUIRED)
    target_link_libraries(${This}-bin ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw glm ${OPENGL_INCLUDE_DIRS} ${DUKTAPE_LIBRARY} ${Vulkan_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto ${ODBC_LIBRARY})
ELSE()
    target_link_libraries(${This}-bin ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw /usr/lib/libduktape.so ${Vulkan_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ODBC_LIBRARY})
ENDIF()