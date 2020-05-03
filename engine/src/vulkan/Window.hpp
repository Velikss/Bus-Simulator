#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/VulkanInstance.hpp>

// Class representing the window which can be used for rendering
class cWindow
{
private:
    cVulkanInstance* ppVulkanInstance;

    // Pointer to the GLFW window instance
    GLFWwindow* ppWindow = nullptr;

    VkSurfaceKHR poSurface = VK_NULL_HANDLE;

public:
    // Window size
    const uint WIDTH = 800;
    const uint HEIGHT = 600;

    cWindow();
    ~cWindow();

    // Create and initialize the window
    void CreateWindow();

    // Create the surface for this window
    bool CreateWindowSurface(cVulkanInstance* pVulkanInstance);

    // Destroy the surface for this window
    // Must be called before destroying the vulkan instance
    void DestroyWindowSurface();

    // Returns true if the window should close
    bool ShouldClose(void);

    // Handles window events
    void HandleEvents(void);

    void SetKeyCallback(GLFWkeyfun pCallback);

    VkSurfaceKHR& GetSurface(void);
};

cWindow::cWindow()
{
    glfwInit();

    // We don't want to create an OpenGL context, so specify NO_API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Window resizing is temporarily disabled
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

cWindow::~cWindow()
{

    // Destroy the GLFW window if it exists
    if (ppWindow != nullptr)
    {
        glfwDestroyWindow(ppWindow);
    }

    // Terminate GLFW
    glfwTerminate();
}

void cWindow::CreateWindow()
{
    assert(ppWindow == nullptr); // don't create a window if it has already been created

    ppWindow = glfwCreateWindow(WIDTH, HEIGHT, "BUS", nullptr, nullptr);
}

bool cWindow::CreateWindowSurface(cVulkanInstance* pVulkanInstance)
{
    assert(pVulkanInstance != nullptr); // vulkan instance should exist

    ppVulkanInstance = pVulkanInstance;

    return pVulkanInstance->CreateWindowSurface(ppWindow, nullptr, &poSurface);
}

void cWindow::DestroyWindowSurface()
{
    assert(ppVulkanInstance != nullptr); // vulkan instance must exist

    ppVulkanInstance->DestroyWindowSurface(poSurface, nullptr);
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

void cWindow::SetKeyCallback(GLFWkeyfun pCallback)
{
    assert(pCallback != nullptr); // callback cannot be null
    assert(ppWindow != nullptr);  // window must be created first

    glfwSetKeyCallback(ppWindow, pCallback);
}

VkSurfaceKHR& cWindow::GetSurface(void)
{
    assert(poSurface != VK_NULL_HANDLE);

    return poSurface;
}
