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

    ppRenderPass = new cRenderPass(ppSwapChain, ppLogicalDevice);

    ppGraphicsPipeline = new cGraphicsPipeline(ppSwapChain, ppLogicalDevice);
}

void Engine::MainLoop(void)
{
    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        ppWindow->MainLoop();
    }
}

void Engine::Cleanup(void)
{
    delete ppSwapChain;
    delete ppLogicalDevice;
    delete ppPhysicalDevice;
    delete ppVulkanInstance;
    delete ppWindow;
}

