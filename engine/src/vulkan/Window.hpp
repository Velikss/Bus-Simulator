#pragma once

//#define QUAD_HD_RESOLUTION

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
#ifdef QUAD_HD_RESOLUTION
    static const uint WIDTH = 2560;
    static const uint HEIGHT = 1380;
#else
    static const uint WIDTH = 1920;
    static const uint HEIGHT = 1080;
#endif

    iInputHandler* ppInputHandler = nullptr;

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

private:
    void FindAndHandleGamepad();
    void HandleGamepad(uint uiJoystickId);

    static void mouseCallback(GLFWwindow* pWindow, double dPosX, double dPosY);
    static void keyCallback(GLFWwindow* pWindow, int iKey, int iScanCode, int iAction, int iMods);
    static void scrollCallback(GLFWwindow* pWindow, double dOffsetX, double dOffsetY);
};

cWindow* cWindow::poInstance = nullptr;

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
    glfwSetInputMode(ppWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(ppWindow, keyCallback);
    glfwSetScrollCallback(ppWindow, scrollCallback);
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

    if (ppInputHandler != nullptr)
    {
        glfwPollEvents();
        FindAndHandleGamepad();
    }
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

void cWindow::FindAndHandleGamepad()
{
    if (poInstance == nullptr || poInstance->ppInputHandler == nullptr) return;

    for (uint uiJoystickId = GLFW_JOYSTICK_1; uiJoystickId < GLFW_JOYSTICK_LAST; uiJoystickId++)
    {
        // Check if the joystick is a gamepad
        if (glfwJoystickIsGamepad(uiJoystickId))
        {
            HandleGamepad(uiJoystickId);
        }
    }
}

void cWindow::HandleGamepad(uint uiJoystickId)
{
    if (poInstance == nullptr || poInstance->ppInputHandler == nullptr) return;

    GLFWgamepadstate tState;
    if (glfwGetGamepadState(uiJoystickId, &tState))
    {
        // Pass the right stick on as mouse input
        ppInputHandler->HandleMouse(tState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
                                    tState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);

        // Temporary mapping for the left stick to keyboard keys
        float fMoveY = tState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
        if (fMoveY < -0.1) ppInputHandler->HandleKey(GLFW_KEY_W, GLFW_PRESS);
        else if (fMoveY > 0.1) ppInputHandler->HandleKey(GLFW_KEY_S, GLFW_PRESS);
        else
        {
            ppInputHandler->HandleKey(GLFW_KEY_W, GLFW_RELEASE);
            ppInputHandler->HandleKey(GLFW_KEY_S, GLFW_RELEASE);
        }

        float fMoveX = tState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        if (fMoveX < -0.1) ppInputHandler->HandleKey(GLFW_KEY_A, GLFW_PRESS);
        else if (fMoveX > 0.1) ppInputHandler->HandleKey(GLFW_KEY_D, GLFW_PRESS);
        else
        {
            ppInputHandler->HandleKey(GLFW_KEY_A, GLFW_RELEASE);
            ppInputHandler->HandleKey(GLFW_KEY_D, GLFW_RELEASE);
        }

        // Temporary mapping for the back, start and menu buttons to the escape key
        if (tState.buttons[GLFW_GAMEPAD_BUTTON_BACK] ||
            tState.buttons[GLFW_GAMEPAD_BUTTON_START] ||
            tState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE])
        {
            ppInputHandler->HandleKey(GLFW_KEY_ESCAPE, GLFW_PRESS);
        }
        else
        {
            ppInputHandler->HandleKey(GLFW_KEY_ESCAPE, GLFW_RELEASE);
        }
    }
}

void cWindow::mouseCallback(GLFWwindow* pWindow, double dPosX, double dPosY)
{
    static bool bFirstMouse = true;
    static float uiLastX = WIDTH, uiLastY = HEIGHT;

    if (poInstance == nullptr || poInstance->ppInputHandler == nullptr) return;

    // If this is the first time the mouse is moved, set
    // the last position to the current
    if (bFirstMouse)
    {
        uiLastX = dPosX;
        uiLastY = dPosY;
        bFirstMouse = false;
    }

    // Calculate the delta between the last and current position
    float uiDeltaX = dPosX - uiLastX;
    float uiDeltaY = dPosY - uiLastY;

    // Set the last position to the current
    uiLastX = dPosX;
    uiLastY = dPosY;

    // Apply a sensitivity factor
    const float fSensitivity = 2.5f;
    uiDeltaX *= fSensitivity;
    uiDeltaY *= fSensitivity;

    // Pass the delta X and Y on to the input handler
    poInstance->ppInputHandler->HandleMouse(uiDeltaX, uiDeltaY);
}

void cWindow::keyCallback(GLFWwindow* pWindow, int iKey, int iScanCode, int iAction, int iMods)
{
    if (poInstance == nullptr || poInstance->ppInputHandler == nullptr) return;

    poInstance->ppInputHandler->HandleKey(iKey, iAction);
}

void cWindow::scrollCallback(GLFWwindow* pWindow, double dOffsetX, double dOffsetY)
{
    if (poInstance == nullptr || poInstance->ppInputHandler == nullptr) return;

    poInstance->ppInputHandler->HandleScroll(dOffsetX, dOffsetY);
}
