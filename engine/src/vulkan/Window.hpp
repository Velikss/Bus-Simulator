#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>

class cWindow
{
private:
    // Pointer to the GLFW window instance
    GLFWwindow* ppWindow = NULL;

public:
    // Window size
    const uint WIDTH = 800;
    const uint HEIGHT = 600;

    cWindow(void);
    ~cWindow(void);

    void CreateWindow(void);
    bool ShouldClose(void);
    void MainLoop(void);

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
    if (ppWindow != NULL)
    {
        glfwDestroyWindow(ppWindow);
    }

    // Terminate GLFW
    glfwTerminate();
}

void cWindow::CreateWindow(void)
{
    assert(ppWindow == NULL); // Don't create a window if it has already been created

    ppWindow = glfwCreateWindow(WIDTH, HEIGHT, "BUS", NULL, NULL);
}

bool cWindow::ShouldClose(void)
{
    assert(ppWindow != NULL); // Window must be created first

    return glfwWindowShouldClose(ppWindow);
}

void cWindow::MainLoop(void)
{
    assert(ppWindow != NULL); // Window must be created first

    glfwPollEvents();
}

bool cWindow::CreateWindowSurface(VkInstance& oVulkanInstance,
                                  VkAllocationCallbacks* pAllocatorCallback,
                                  VkSurfaceKHR* pSurface)
{
    assert(pSurface != NULL);

    return glfwCreateWindowSurface(oVulkanInstance, ppWindow, pAllocatorCallback, pSurface) == VK_SUCCESS;
}
