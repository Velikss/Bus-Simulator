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

    cLogicalDevice* ppLogicalDevice;

    cSurface* ppSurface;
    cSwapChain* ppSwapChain;
    cUniformHandler* ppUniformHandler;
    cGraphicsPipeline* ppGraphicsPipeline;
    cRenderPass* ppRenderPass;

    cCommandHandler* ppCommandHandler;
    cTextureHandler* ppTextureHandler;
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
    cPhysicalDevice::GetInstance()->SelectPhysicalDevice(ppVulkanInstance, ppSurface);

    // Create the logical device. This is the class used to interface with the physical device.
    ppLogicalDevice = new cLogicalDevice();

    // Create the swap chain. The swap chain holds the frames before we present them to the screen
    ppSwapChain = new cSwapChain(ppLogicalDevice, ppWindow, ppSurface);

    // Create the render pass. This holds information about the frames we want to render.
    // will probably be moved somewhere else later
    ppRenderPass = new cRenderPass(ppSwapChain, ppLogicalDevice);

    // Create the uniform handler. This is responsible for the uniform variables
    ppUniformHandler = new cUniformHandler(ppLogicalDevice, ppSwapChain);

    // Create the graphics pipeline. Handles the shaders and fixed-function operations for the graphics pipeline.
    ppGraphicsPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice, ppRenderPass, ppUniformHandler);

    // Create the command handler. This deals with the commands that tell Vulkan what to do
    ppCommandHandler = new cCommandHandler(ppLogicalDevice);

    ppTextureHandler = new cTextureHandler(ppLogicalDevice);

    ppSwapChain->CreateDepthResources();

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppRenderPass->poRenderPass);

    // Create the vertex buffer. This stores the vertex data and handles copying it to the GPU memory
    ppVertexBuffer = new cVertexBuffer(ppLogicalDevice, cCommandHelper::poCommandPool);

    ppUniformHandler->SetupUniformBuffers();

    // Create and record the command buffers. A sequence of commands has to be recorded before you can to use them
    ppCommandHandler->CreateCommandBuffers(ppSwapChain, ppRenderPass, ppGraphicsPipeline,
                                           ppVertexBuffer, ppUniformHandler);

    // Create the rendering handler. Acquires the frames from the swapChain, submits them to the graphics queue
    // to execute the commands, then submits them to the presentation queue to show them on the screen
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, ppCommandHandler, ppUniformHandler);
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
    delete ppUniformHandler;
    delete ppRenderPass;
    delete ppSwapChain;
    delete ppLogicalDevice;
    delete ppVulkanInstance;
    delete ppWindow;
}

