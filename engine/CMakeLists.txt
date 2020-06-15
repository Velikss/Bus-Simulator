cmake_minimum_required(VERSION 3.13.4)

set(This engine)
project(${This} C CXX)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -luuid")

find_package(GTest CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig)
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

get_target_property(GLFW_INCLUDE_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
IF (WIN32)
    add_library(fmod STATIC IMPORTED)

    find_package(glm REQUIRED)
    find_package(duktape REQUIRED)
    get_target_property(GLM_INCLUDE_DIR glm INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(GLFW_LIBRARIES glfw IMPORTED_IMPLIB_RELEASE)

    set(_ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/fmod/windows/api/core/inc)
    set(_ENGINE_LIBRARIES ${PROJECT_SOURCE_DIR}/vendor/fmod/windows/api/core/lib/x64/fmod_vc.lib ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${DUKTAPE_LIBRARY} ${Vulkan_LIBRARIES} ${OPENSSL_SSL_LIBRARIES} ${OPENSSL_CRYPTO_LIBRARIES} ${ODBC_LIBRARY})
ELSE ()
    add_library(fmod SHARED IMPORTED)

    get_target_property(GLFW_LIBRARIES glfw IMPORTED_LOCATION_NOCONFIG)
    set(_ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/fmod/linux/api/core/inc)
    set(_ENGINE_LIBRARIES /usr/lib/fmod/libfmod.so ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} /usr/lib/libduktape.so ${Vulkan_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ${ODBC_LIBRARY})
ENDIF ()
list(APPEND _ENGINE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src ${Vulkan_INCLUDE_DIRS} ${GLM_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIR} ${DUKTAPE_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR} /usr/lib/libuuid.a)
set(ENGINE_INCLUDE_DIRS ${_ENGINE_INCLUDE_DIRS} PARENT_SCOPE)
set(ENGINE_LIBRARIES ${_ENGINE_LIBRARIES} PARENT_SCOPE)

set(Headers
        ${PROJECT_SOURCE_DIR}/src/pch.hpp
        ${PROJECT_SOURCE_DIR}/vendor/tiny_obj_loader.h
        ${PROJECT_SOURCE_DIR}/vendor/stb_image.h
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
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/Scene.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/BaseObject.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/InputHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/PipelineHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/uniform/UniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/renderpass/RenderPass.hpp
        ${PROJECT_SOURCE_DIR}/src/util/Formatter.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/RenderModule.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/pipeline/RenderPipeline.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/swapchain/SwapChainHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/swapchain/SwapChain.hpp
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
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/text/Font.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/loop/GameLoop.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/loop/TickTask.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/scene/LightObject.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/EngineLog.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/geometry/ViewportQuadGeometry.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/texture/TextureSampler.hpp
        ${PROJECT_SOURCE_DIR}/vendor/tiny_obj_loader.h
        ${PROJECT_SOURCE_DIR}/vendor/stb_image.h
        ${PROJECT_SOURCE_DIR}/src/scripting/ScriptingEngine.hpp
        ${PROJECT_SOURCE_DIR}/src/scripting/functions/JavaScriptBaseFunctions.hpp
        ${PROJECT_SOURCE_DIR}/src/scripting/functions/JavaScriptEntityFunctions.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/BaseEntity.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/Entity.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/EntityGroup.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/entities/BehaviourHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/collision/Collider.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/collision/CollisionHelper.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/collision/ColliderSet.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/StaticElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/TextElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/AudioHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/DirectoryWatcher.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/OverlayWindow.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/GameManager.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/ClickableElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/UIManager.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/UIElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/Invalidatable.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/CommandBufferHolder.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/CommandRecorderProvider.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/DynamicUniformHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/SimpleButton.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/CompoundElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/TextBoxElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/FocusHandler.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/Focussable.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/PasswordTextBox.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/TabsElement.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/ComboBox.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/ValueSelector.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/module/overlay/element/elements/CheckBox.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/SceneManager.hpp
        ${PROJECT_SOURCE_DIR}/src/vulkan/util/Settings.hpp src/vulkan/util/Profiler.hpp src/vulkan/geometry/GeometryHandler.hpp src/vulkan/util/AsyncLoader.hpp)

include_directories(${_ENGINE_INCLUDE_DIRS})
add_library(${This} STATIC ${PROJECT_SOURCE_DIR}/src/main.cpp ${Headers})
add_executable(${This}-Test ${PROJECT_SOURCE_DIR}/test/main.cpp ${Headers})
target_include_directories(${This} INTERFACE ${_ENGINE_INCLUDE_DIRS})
target_include_directories(${This}-Test INTERFACE ${_ENGINE_INCLUDE_DIRS})

IF (WIN32)
    target_link_libraries(${This} PUBLIC ${_ENGINE_LIBRARIES} glm)
    target_link_libraries(${This}-Test PUBLIC ${_ENGINE_LIBRARIES} glm GTest::gtest)
ELSE ()
    target_link_libraries(${This} PUBLIC ${_ENGINE_LIBRARIES})
    target_link_libraries(${This}-Test PUBLIC ${_ENGINE_LIBRARIES} gtest)
ENDIF ()

add_test(
        NAME ${This}-Test
        COMMAND ${This}-Test
)