#pragma once

#define ENABLE_OVERLAY
#define ENABLE_FPS_COUNT
//#define ENGINE_TIMING_DEBUG
#define ENGINE_ENABLE_LOG

#include <pch.hpp>
#include <vulkan/util/EngineLog.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstring>
#include <optional>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/VulkanInstance.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/lighting/LightingPipeline.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/RenderHandler.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/command/CommandBuffer.hpp>
#include <vulkan/module/mrt/MRTRenderRecorder.hpp>
#include <vulkan/command/ClearScreenRecorder.hpp>
#include <vulkan/scene/TestScene.hpp>
#include <vulkan/scene/StreetScene.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/lighting/LightingRenderModule.hpp>
#include <vulkan/module/mrt/MRTRenderModule.hpp>
#include <vulkan/module/lighting/LightingRenderRecorder.hpp>
#include <vulkan/loop/GameLoop.hpp>
#include <vulkan/scene/BusWorldScene.hpp>
#include <thread>
#include <chrono>

class Engine
{
private:
    cWindow* ppWindow;
    cVulkanInstance* ppVulkanInstance;

    cLogicalDevice* ppLogicalDevice;
    cSwapChain* ppSwapChain;

    cRenderModule* ppLightsRenderModule;

#ifdef ENABLE_OVERLAY
    cCommandBuffer* papCommandBuffers[3];
    iUniformHandler* papUniformHandlers[3];
#else
    cCommandBuffer* papCommandBuffers[2];
    iUniformHandler* papUniformHandlers[2];
#endif

    cTextureHandler* ppTextureHandler;
    cRenderHandler* ppRenderHandler;

    cOverlayRenderModule* ppOverlayRenderModule = nullptr;
    cMRTRenderModule* ppMRTRenderModule;

    cScene* ppScene = nullptr;

    cGameLoop* ppGameLoop;
    std::thread* ppGameThread;

public:
    // Initializes and starts the engine and all of it's sub-components
    void Run(void);

private:
    void CreateGLWindow(void);
    void InitVulkan(void);
    void MainLoop(void);
    void Cleanup(void);
};

void Engine::Run()
{
    CreateGLWindow();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void Engine::CreateGLWindow(void)
{
    ppWindow = new cWindow();
    ppWindow->CreateGLWindow();
}

void Engine::InitVulkan(void)
{
    ENGINE_LOG("Initializing engine...");

    // Create the Vulkan instance
    ppVulkanInstance = new cVulkanInstance();

    // Create the window surface. This is the part of the window that we will be drawing to
    ppWindow->CreateWindowSurface(ppVulkanInstance);

    // Setup a physical graphics device. This will find a physical GPU that supports the things
    // we need and store information about it
    cPhysicalDevice::GetInstance()->SelectPhysicalDevice(ppVulkanInstance, ppWindow);

    // Create the logical device. This is the class used to interface with the physical device.
    ppLogicalDevice = new cLogicalDevice();

    // Create the swap chain. The swap chain holds the frames before we present them to the screen
    ppSwapChain = new cSwapChain(ppLogicalDevice, ppWindow);

    // Setup the command pool
    cCommandHelper::SetupCommandPool(ppLogicalDevice);

    // Create and setup the depth resources
    ppSwapChain->CreateResources();

    ppLightsRenderModule = new cLightingRenderModule(ppLogicalDevice, ppSwapChain);
    ppMRTRenderModule = new cMRTRenderModule(ppLogicalDevice, ppSwapChain);
#ifdef ENABLE_OVERLAY
    ppOverlayRenderModule = new cOverlayRenderModule(ppLogicalDevice, ppSwapChain, ppWindow);

    ENGINE_LOG("Overlay is enabled!");
#endif

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppLightsRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppMRTRenderModule->GetRenderPass()->GetRenderPass(),
#ifdef ENABLE_OVERLAY
                                    ppOverlayRenderModule->GetRenderPass()->GetRenderPass());
#else
                                    ppMRTRenderModule->GetRenderPass()->GetRenderPass());
#endif

    // Create two command buffers, one for the graphics, one for the overlay
    papCommandBuffers[0] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
    papCommandBuffers[1] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
#ifdef ENABLE_OVERLAY
    papCommandBuffers[2] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
#endif

    // Get the two uniform handlers
    papUniformHandlers[0] = ppLightsRenderModule->GetUniformHandler();
    papUniformHandlers[1] = ppMRTRenderModule->GetUniformHandler();
#ifdef ENABLE_OVERLAY
    papUniformHandlers[2] = ppOverlayRenderModule->GetUniformHandler();
#endif

    // Create the rendering handler. Acquires the frames from the swapChain, submits them to the graphics queue
    // to execute the commands, then submits them to the presentation queue to show them on the screen
#ifdef ENABLE_OVERLAY
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, papCommandBuffers, 3);
    ppRenderHandler->SetUniformHandlers(papUniformHandlers, 3);
#else
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, papCommandBuffers, 2);
    ppRenderHandler->SetUniformHandlers(papUniformHandlers, 2);
#endif

    // Create the texture handler. This deals with loading, binding and sampling the textures
    ppTextureHandler = new cTextureHandler(ppLogicalDevice);

    // Record a clear screen to the graphics command buffer
    cClearScreenRecorder clearRecorder(ppLightsRenderModule->GetRenderPass(), ppSwapChain);
    papCommandBuffers[0]->RecordBuffers(&clearRecorder);
    papCommandBuffers[1]->RecordBuffers(&clearRecorder);

#ifdef ENABLE_OVERLAY
    // Record the overlay to the overlay command buffer
    papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());
#endif

    ppGameLoop = new cGameLoop();
    ppGameThread = new std::thread(std::ref(*ppGameLoop));

    ENGINE_LOG("Engine initialized");
}

void Engine::MainLoop(void)
{
    ENGINE_LOG("Engine running");

    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        // Let the window do it's thing
        ppWindow->HandleEvents();

        if (ppScene != nullptr)
        {
            // If the scene is signalling it wants to
            // quit, pass it on to the window
            if (ppScene->ShouldQuit())
            {
                ENGINE_LOG("Scene is asking for application quit");
                ppWindow->Close();
            }
        }

        // If the scene hasn't been loaded, load it now
        // We want to draw at least one frame before loading the
        // scene, to allow loading text to be displayed
        if (ppScene == nullptr)
        {
            ENGINE_LOG("Loading scene...");

            // Create and load the scene
            ppScene = new cBusWorldScene();
            ppScene->Load(ppTextureHandler, ppLogicalDevice);

            // The scene will handle the input
            ppWindow->ppInputHandler = ppScene;

            // Setup the buffers for uniform variables
            ppLightsRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);
            ppMRTRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);

            // We cannot (re-)record command buffers while the GPU is
            // using them, so we have to wait until it's idle.
            ppLogicalDevice->WaitUntilIdle();

            // Record the commands for rendering to the command buffer.
            cMRTRenderRecorder mrt(ppMRTRenderModule->GetRenderPass(), ppSwapChain,
                                   ppMRTRenderModule->GetRenderPipeline(),
                                   ppMRTRenderModule->GetUniformHandler(), ppScene);
            cLightingRenderRecorder light(ppLightsRenderModule->GetRenderPass(), ppSwapChain,
                                          ppLightsRenderModule->GetRenderPipeline(),
                                          ppLightsRenderModule->GetUniformHandler(), ppScene);
            papCommandBuffers[0]->RecordBuffers(&mrt);
            papCommandBuffers[1]->RecordBuffers(&light);

            ENGINE_LOG("Scene loading, adding tick task");
            ppGameLoop->AddTask(ppScene);
        }

        // Draw a frame
        ppRenderHandler->DrawFrame(ppScene, ppOverlayRenderModule, papCommandBuffers[2]);
    }

    ppGameLoop->Stop();

    ENGINE_LOG("Main loop closed");

    // Wait until the logical device is idle before returning
    ppLogicalDevice->WaitUntilIdle();
}

void Engine::Cleanup(void)
{
    ENGINE_LOG("Cleaning up engine...");

    // Clean up the game thread
    ppGameThread->join();
    delete ppGameThread;
    delete ppGameLoop;

    delete ppScene;
    delete ppRenderHandler;
#ifdef ENABLE_OVERLAY
    delete ppOverlayRenderModule;
#endif
    delete ppTextureHandler;
    for (auto oBuffer : papCommandBuffers)
    {
        delete oBuffer;
    }
    ppLogicalDevice->DestroyCommandPool(cCommandHelper::poCommandPool, nullptr);
    delete ppMRTRenderModule;
    delete ppLightsRenderModule;
    delete ppSwapChain;
    delete ppLogicalDevice;
    ppWindow->DestroyWindowSurface(); // surface must be destroyed before the instance
    delete ppVulkanInstance;
    delete ppWindow;

    ENGINE_LOG("Engine shut down cleanly");
}


