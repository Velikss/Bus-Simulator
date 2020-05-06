#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/VulkanInstance.hpp>
#include <vulkan/scene/InputHandler.hpp>

// Class representing the window which can be used for rendering
class cWindow
{
private:
    static cWindow* poInstance;

    cVulkanInstance* ppVulkanInstance;


    VkSurfaceKHR poSurface = VK_NULL_HANDLE;

public:
    // Pointer to the GLFW window instance
    GLFWwindow* ppWindow = nullptr;

    // Window size
    static const uint WIDTH = 2500;
    static const uint HEIGHT = 1300;

    iInputHandler* ppInputHandler;

    cWindow();
    ~cWindow();

    // Create and initialize the window
    void CreateGLWindow();

    // Create the surface for this window
    bool CreateWindowSurface(cVulkanInstance* pVulkanInstance);

    // Destroy the surface for this window
    // Must be called before destroying the vulkan instance
    void DestroyWindowSurface();

    // Returns true if the window should close
    bool ShouldClose(void);

    // Handles window events
    void HandleEvents(void);

    // Mark this window as should close
    void Close(void);

    VkSurfaceKHR& GetSurface(void);

    static void mouseCallback(GLFWwindow* pWindow, double dPosX, double dPosY);
    static void keyCallback(GLFWwindow* pWindow, int iKey, int iScanCode, int iAction, int iMods);
};

cWindow* cWindow::poInstance;

cWindow::cWindow()
{
    glfwInit();

    // We don't want to create an OpenGL context, so specify NO_API
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Window resizing is temporarily disabled
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    poInstance = this;
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

void cWindow::CreateGLWindow()
{
    assert(ppWindow == nullptr); // don't create a window if it has already been created

    ppWindow = glfwCreateWindow(WIDTH, HEIGHT, "BUS", nullptr, nullptr);

    glfwSetCursorPosCallback(ppWindow, mouseCallback);
    //glfwSetInputMode(ppWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(ppWindow, keyCallback);
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

void cWindow::Close(void)
{
    glfwSetWindowShouldClose(ppWindow, GLFW_TRUE);
}

VkSurfaceKHR& cWindow::GetSurface(void)
{
    assert(poSurface != VK_NULL_HANDLE);

    return poSurface;
}

void cWindow::mouseCallback(GLFWwindow* pWindow, double dPosX, double dPosY)
{
    static bool bFirstMouse = true;
    static float uiLastX = WIDTH, uiLastY = HEIGHT;

    if (bFirstMouse)
    {
        uiLastX = dPosX;
        uiLastY = dPosY;
        bFirstMouse = false;
    }

    float uiDeltaX = dPosX - uiLastX;
    float uiDeltaY = dPosY - uiLastY;
    uiLastX = dPosX;
    uiLastY = dPosY;

    const float fSensitivity = 2.5f;
    uiDeltaX *= fSensitivity;
    uiDeltaY *= fSensitivity;

    poInstance->ppInputHandler->HandleMouse(uiDeltaX, uiDeltaY);
}

void cWindow::keyCallback(GLFWwindow* pWindow, int iKey, int iScanCode, int iAction, int iMods)
{
    poInstance->ppInputHandler->HandleKey(iKey, iAction);
}
