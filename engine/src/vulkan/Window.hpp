#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>

// Class representing the window which can be used for rendering
class cWindow
{
private:
    // Pointer to the GLFW window instance
    GLFWwindow* ppWindow = nullptr;

public:
    // Window size
    const uint WIDTH = 800;
    const uint HEIGHT = 600;

    cWindow(void);
    ~cWindow(void);

    // Create and initialize the window
    void CreateWindow(void);

    // Returns true if the window should close
    bool ShouldClose(void);

    // Handles window events
    void HandleEvents(void);

    // Create the surface for this window
    bool CreateWindowSurface(VkInstance& oVulkanInstance,
                             VkAllocationCallbacks* pAllocatorCallback,
                             VkSurfaceKHR* pSurface);
};

cWindow::cWindow(void)
{
    glfwInit();

    // We don't want to create an OpenGL context, so specify NO_API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Window resizing is temporarily disabled
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

cWindow::~cWindow(void)
{
    // Destroy the GLFW window if it exists
    if (ppWindow != nullptr)
    {
        glfwDestroyWindow(ppWindow);
    }

    // Terminate GLFW
    glfwTerminate();
}

void cWindow::CreateWindow(void)
{
    assert(ppWindow == nullptr); // don't create a window if it has already been created

    ppWindow = glfwCreateWindow(WIDTH, HEIGHT, "BUS", nullptr, nullptr);
}

bool cWindow::ShouldClose(void)
{
    assert(ppWindow != nullptr); // window must be created first

    return glfwWindowShouldClose(ppWindow);
}

void cWindow::HandleEvents(void)
{
    assert(ppWindow != nullptr); // window must be created first

    glfwPollEvents();
}

bool cWindow::CreateWindowSurface(VkInstance& oVulkanInstance,
                                  VkAllocationCallbacks* pAllocatorCallback,
                                  VkSurfaceKHR* pSurface)
{
    assert(oVulkanInstance != VK_NULL_HANDLE); // vulkan instance should exist
    assert(pSurface != nullptr); // pSurface should point to where the surface needs to be stored

    return glfwCreateWindowSurface(oVulkanInstance, ppWindow, pAllocatorCallback, pSurface) == VK_SUCCESS;
}
