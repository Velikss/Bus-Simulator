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
    // Initializes and starts the engine and all of it's sub-components
    void Run(void);

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

    // Create the window surface. This is the part of the window that we will be drawing to
    ppSurface = new cSurface(ppVulkanInstance, ppWindow);

    // Setup a physical graphics device. This will find a physical GPU that supports the things
    // we need and store information about it
    ppPhysicalDevice = new cPhysicalDevice(ppVulkanInstance, ppSurface);

    // Create the logical device. This is the class used to interface with the physical device.
    ppLogicalDevice = new cLogicalDevice(ppPhysicalDevice);

    // Create the swap chain. The swap chain holds the frames before we present them to the screen
    ppSwapChain = new cSwapChain(ppPhysicalDevice, ppLogicalDevice, ppWindow, ppSurface);

    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cRenderPass(ppSwapChain, ppLogicalDevice);

    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppGraphicsPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass);

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppRenderPass->poRenderPass);

    // Create the command handler. This deals with the commands that tell Vulkan what to do
    ppCommandHandler = new cCommandHandler(ppPhysicalDevice, ppLogicalDevice);

    // Create the vertex buffer. This stores the vertex data and handles copying it to the GPU memory
    ppVertexBuffer = new cVertexBuffer(ppLogicalDevice, ppPhysicalDevice, ppCommandHandler->poCommandPool);

    // Create and record the command buffers. A sequence of commands has to be recorded before you can to use them
    ppCommandHandler->CreateCommandBuffers(ppSwapChain, ppRenderPass, ppGraphicsPipeline, ppVertexBuffer);

    // Create the rendering handler. Acquires the frames from the swapChain, submits them to the graphics queue
    // to execute the commands, then submits them to the presentation queue to show them on the screen
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

