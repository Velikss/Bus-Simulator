#pragma once

#define ENABLE_FPS_COUNT

#include <pch.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstring>
#include <optional>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/VulkanInstance.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/pipeline/GraphicsPipeline.hpp>
#include <vulkan/GraphicsRenderPass.hpp>
#include <vulkan/RenderHandler.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/command/CommandBuffer.hpp>
#include <vulkan/command/IndexedRenderRecorder.hpp>
#include <vulkan/command/ClearScreenRecorder.hpp>
#include <vulkan/scene/TestScene.hpp>
#include <vulkan/scene/StreetScene.hpp>
#include <vulkan/overlay/TextHandler.hpp>

class Engine
{
private:
    cWindow* ppWindow;
    cVulkanInstance* ppVulkanInstance;

    cLogicalDevice* ppLogicalDevice;

    cSwapChain* ppSwapChain;
    cGraphicsUniformHandler* ppUniformHandler;
    cGraphicsPipeline* ppGraphicsPipeline;
    cRenderPass* ppRenderPass;

    cCommandBuffer* papCommandBuffers[2];
    iUniformHandler* papUniformHandlers[2];
    cTextureHandler* ppTextureHandler;
    cRenderHandler* ppRenderHandler;

    cScene* ppScene = nullptr;
    cTextHandler* ppTextHandler;

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

    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cGraphicsRenderPass(ppLogicalDevice, ppSwapChain);

    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cGraphicsUniformHandler(ppLogicalDevice, ppSwapChain);

    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppGraphicsPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler);

    // Setup the command pool
    cCommandHelper::SetupCommandPool(ppLogicalDevice);

    // Create and setup the depth resources
    ppSwapChain->CreateDepthResources();

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppRenderPass->poRenderPass);

    ppTextHandler = new cTextHandler(ppLogicalDevice, ppSwapChain, ppWindow);

    papCommandBuffers[0] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
    papCommandBuffers[1] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);

    papUniformHandlers[0] = ppUniformHandler;
    papUniformHandlers[1] = ppTextHandler->GetUniformHandler();

    // Create the rendering handler. Acquires the frames from the swapChain, submits them to the graphics queue
    // to execute the commands, then submits them to the presentation queue to show them on the screen
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, papCommandBuffers, 2);
    ppRenderHandler->SetUniformHandlers(papUniformHandlers, 2);

    // Create the texture handler. This deals with loading, binding and sampling the textures
    ppTextureHandler = new cTextureHandler(ppLogicalDevice);

    cClearScreenRecorder clearRecorder(ppRenderPass, ppSwapChain);
    papCommandBuffers[0]->RecordBuffers(&clearRecorder);

    papCommandBuffers[1]->RecordBuffers(ppTextHandler->GetCommandRecorder());
}

void Engine::MainLoop(void)
{
    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        // Let the window do it's thing
        ppWindow->HandleEvents();

        if (ppScene != nullptr)
        {
            // Update the scene
            ppScene->Update();

            // If the scene is signalling it wants to
            // quit, pass it on to the window
            if (ppScene->ShouldQuit())
            {
                ppWindow->Close();
            }
        }

        // Draw a frame
        ppRenderHandler->DrawFrame(ppScene, ppTextHandler, papCommandBuffers[1]);

        // If the scene hasn't been loaded, load it now
        // We want to draw at least one frame before loading the
        // scene, to allow loading text to be displayed
        if (ppScene == nullptr)
        {
            // Create and load the scene
            ppScene = new cStreetScene();
            ppScene->Load(ppTextureHandler, ppLogicalDevice);

            // The scene will handle the input
            ppWindow->ppInputHandler = ppScene;

            // Setup the buffers for uniform variables
            ppUniformHandler->SetupUniformBuffers(ppTextureHandler, ppScene);

            // We cannot (re-)record command buffers while the GPU is
            // using them, so we have to wait until it's idle.
            ppLogicalDevice->WaitUntilIdle();

            // Record the commands for rendering to the command buffer.
            cIndexedRenderRecorder recorder(ppRenderPass, ppSwapChain, ppGraphicsPipeline,
                                            ppUniformHandler, ppScene);
            papCommandBuffers[0]->RecordBuffers(&recorder);
        }
    }

    // Wait until the logical device is idle before returning
    ppLogicalDevice->WaitUntilIdle();
}

void Engine::Cleanup(void)
{
    delete ppScene;
    delete ppRenderHandler;
    delete ppTextHandler;
    delete ppTextureHandler;
    for (auto oBuffer : papCommandBuffers)
    {
        delete oBuffer;
    }
    ppLogicalDevice->DestroyCommandPool(cCommandHelper::poCommandPool, nullptr);
    delete ppGraphicsPipeline;
    delete ppUniformHandler;
    delete ppRenderPass;
    delete ppSwapChain;
    delete ppLogicalDevice;
    ppWindow->DestroyWindowSurface(); // surface must be destroyed before the instance
    delete ppVulkanInstance;
    delete ppWindow;
}


