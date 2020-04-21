#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstring>
#include <optional>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/VulkanInstance.hpp>
#include <vulkan/Surface.hpp>
#include <vulkan/SwapChain.hpp>
#include <vulkan/GraphicsPipeline.hpp>
#include <vulkan/RenderPass.hpp>
#include <vulkan/CommandHandler.hpp>
#include <vulkan/RenderHandler.hpp>
#include <vulkan/VertexBuffer.hpp>

class Engine
{
private:
    cWindow* ppWindow;
    cVulkanInstance* ppVulkanInstance;

    cPhysicalDevice* ppPhysicalDevice;
    cLogicalDevice* ppLogicalDevice;

    cSurface* ppSurface;
    cSwapChain* ppSwapChain;
    cGraphicsPipeline* ppGraphicsPipeline;
    cRenderPass* ppRenderPass;

    cCommandHandler* ppCommandHandler;
    cRenderHandler* ppRenderHandler;
    cVertexBuffer* ppVertexBuffer;

public:
    // Window size
    const uint WIDTH = 800;
    const uint HEIGHT = 600;

    // Validation layers are hooks inside of Vulkan that can be used for debugging
    const std::vector<const char *> VALIDATION_LAYERS = {
    };

    // Disable validation layers when not debugging
#ifdef NDEBUG
    const bool ENABLE_VALIDATION_LAYERS = false;
#else
    const bool ENABLE_VALIDATION_LAYERS = true;
#endif

    // Initializes and starts the engine and all of it's sub-components
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    void CreateWindow(void);
    void InitVulkan(void);
    void MainLoop(void);
    void Cleanup(void);
};

void Engine::Run()
{
    CreateWindow();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void Engine::CreateWindow(void)
{
    ppWindow = new cWindow();
    ppWindow->CreateWindow();
}

void Engine::InitVulkan(void)
{
    // Create the Vulkan instance
    ppVulkanInstance = new cVulkanInstance();

    // Create the window surface
    ppSurface = new cSurface(ppVulkanInstance, ppWindow);

    // Setup a physical graphics device
    ppPhysicalDevice = new cPhysicalDevice(ppVulkanInstance, ppSurface);

    // Create the logical device
    ppLogicalDevice = new cLogicalDevice(ppPhysicalDevice);

    // Create the swap chain
    ppSwapChain = new cSwapChain(ppPhysicalDevice, ppLogicalDevice, ppWindow, ppSurface);

    // Create the render pass
    ppRenderPass = new cRenderPass(ppSwapChain, ppLogicalDevice);

    // Create the graphics pipeline
    ppGraphicsPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass);

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppRenderPass->poRenderPass);

    // Create the command handler
    ppCommandHandler = new cCommandHandler(ppPhysicalDevice, ppLogicalDevice);

    // Create the vertex buffer
    ppVertexBuffer = new cVertexBuffer(ppLogicalDevice, ppPhysicalDevice);

    // Create and record the command buffers
    ppCommandHandler->CreateCommandBuffers(ppSwapChain, ppRenderPass, ppGraphicsPipeline, ppVertexBuffer);

    // Create the rendering handler
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, ppCommandHandler);
}

void Engine::MainLoop(void)
{
    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        // Let the window do it's thing
        ppWindow->MainLoop();

        // Draw a frame
        ppRenderHandler->DrawFrame();
    }

    // Wait until the logical device is idle before returning
    ppLogicalDevice->WaitUntilIdle();
}

void Engine::Cleanup(void)
{
    delete ppRenderHandler;
    delete ppVertexBuffer;
    delete ppCommandHandler;
    delete ppGraphicsPipeline;
    delete ppRenderPass;
    delete ppSwapChain;
    delete ppLogicalDevice;
    delete ppPhysicalDevice;
    delete ppVulkanInstance;
    delete ppWindow;
}

