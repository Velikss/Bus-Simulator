#pragma once

#define ENABLE_FPS_COUNT
//#define ENGINE_TIMING_DEBUG
#define ENGINE_ENABLE_LOG

#include <pch.hpp>
#include <vulkan/util/EngineLog.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstring>
#include <optional>
#include <vulkan/Window.hpp>
#include <vulkan/PhysicalDevice.hpp>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/VulkanInstance.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/module/lighting/LightingPipeline.hpp>
#include <vulkan/module/lighting/LightingRenderPass.hpp>
#include <vulkan/RenderHandler.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/command/CommandBuffer.hpp>
#include <vulkan/module/mrt/MRTRenderRecorder.hpp>
#include <vulkan/command/ClearScreenRecorder.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/lighting/LightingRenderModule.hpp>
#include <vulkan/module/mrt/MRTRenderModule.hpp>
#include <vulkan/module/lighting/LightingRenderRecorder.hpp>
#include <vulkan/loop/GameLoop.hpp>
#include <vulkan/AudioHandler.hpp>
#include <thread>
#include <chrono>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/OverlayProvider.hpp>
#include <vulkan/util/CommandBufferHolder.hpp>

class cEngine : public iOverlayProvider, public iInputHandler, public iCommandBufferHolder
{
private:
    const string psAppName;

    cWindow* ppWindow = nullptr;
    cVulkanInstance* ppVulkanInstance = nullptr;

    cLogicalDevice* ppLogicalDevice = nullptr;
    cSwapChain* ppSwapChain = nullptr;
    cTextureHandler* ppTextureHandler = nullptr;

    cRenderHandler* ppRenderHandler = nullptr;

    cMRTRenderModule* ppMRTRenderModule = nullptr;
    cOverlayRenderModule* ppOverlayRenderModule = nullptr;
    cRenderModule* ppLightsRenderModule = nullptr;

    cCommandBuffer* papCommandBuffers[3];
    iUniformHandler* papUniformHandlers[3];

    cGameLoop* ppGameLoop = nullptr;
    std::thread* ppGameThread = nullptr;

    cScene* ppScene = nullptr;
    cAudioHandler* ppAudioHandler = nullptr;

    std::map<string, cOverlayWindow*> pmOverlayWindows;
    cOverlayWindow* ppActiveOverlayWindow = nullptr;
    cOverlayWindow* ppRequestedOverlayWindow = nullptr;
    bool pbUpdateOverlayWindow = false;

    bool pbInitialized = false;

public:
    cEngine(const string& sAppName = "");

    // Initializes and starts the engine and all of it's sub-components
    void Run(void);

    cOverlayWindow* GetActiveOverlayWindow() override;
    void ActivateOverlayWindow(const string& sName) override;
    void DeactivateOverlayWindow() override;

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;
    void HandleScroll(double dOffsetX, double dOffsetY) override;
    void HandleCharacter(char cCharacter) override;
    cCommandBuffer** GetCommandBuffers() override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

protected:
    virtual void LoadMRTShaders(std::vector<string>& shaders) = 0;
    virtual void LoadLightingShaders(std::vector<string>& shaders) = 0;
    virtual void LoadOverlayShaders(std::vector<string>& shaders) = 0;

    virtual void LoadOverlayWindows(std::map<string, cOverlayWindow*>& mOverlayWindows) = 0;
    virtual void LoadScene(cScene** pScene) = 0;

private:
    void CreateGLWindow(void);
    void InitAudio(void);
    void InitVulkan(void);
    void MainLoop(void);
    void Cleanup(void);
    void RebuildPipeline(void);
};

cEngine::cEngine(const string& sAppName) : psAppName(sAppName)
{
}

void cEngine::Run()
{
    CreateGLWindow();
    InitAudio();
    InitVulkan();
    MainLoop();
    Cleanup();
}

void cEngine::CreateGLWindow(void)
{
    ppWindow = new cWindow(psAppName);
    ppWindow->CreateGLWindow();
    ppWindow->ppInputHandler = this;
}

void cEngine::InitAudio(void)
{
    ENGINE_LOG("Initializing audio system...");

    ppAudioHandler = new cAudioHandler();
}

void cEngine::InitVulkan(void)
{
    ENGINE_LOG("Initializing engine...");

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

    // Setup the command pool
    cCommandHelper::SetupCommandPool(ppLogicalDevice);

    // Create and setup the depth resources
    ppSwapChain->CreateResources();

    ENGINE_LOG("Creating render pipeline...");

    // Setup the render modules
    std::vector<string> aMRTShaders;
    LoadMRTShaders(aMRTShaders);
    ppMRTRenderModule = new cMRTRenderModule(ppLogicalDevice, ppSwapChain, aMRTShaders);

    std::vector<string> aLightingShaders;
    LoadLightingShaders(aLightingShaders);
    ppLightsRenderModule = new cLightingRenderModule(ppLogicalDevice, ppSwapChain, aLightingShaders);

    std::vector<string> aOverlayShaders;
    LoadOverlayShaders(aOverlayShaders);
    ppOverlayRenderModule = new cOverlayRenderModule(ppLogicalDevice, ppSwapChain, ppWindow,
                                                     aOverlayShaders, this);

    // Create the framebuffers for the swap chain
    ppSwapChain->CreateFramebuffers(ppLightsRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppMRTRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppOverlayRenderModule->GetRenderPass()->GetRenderPass());

    // Create two command buffers, one for the graphics, one for the overlay
    papCommandBuffers[0] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
    papCommandBuffers[1] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);
    papCommandBuffers[2] = new cCommandBuffer(ppLogicalDevice, ppSwapChain);

    // Get the two uniform handlers
    papUniformHandlers[0] = ppLightsRenderModule->GetUniformHandler();
    papUniformHandlers[1] = ppMRTRenderModule->GetUniformHandler();
    papUniformHandlers[2] = ppOverlayRenderModule->GetUniformHandler();

    // Create the rendering handler. Acquires the frames from the swapChain, submits them to the graphics queue
    // to execute the commands, then submits them to the presentation queue to show them on the screen
    ppRenderHandler = new cRenderHandler(ppLogicalDevice, ppSwapChain, papCommandBuffers);
    ppRenderHandler->SetUniformHandlers(papUniformHandlers, 3);

    // Create the texture handler. This deals with loading, binding and sampling the textures
    ppTextureHandler = new cTextureHandler(ppLogicalDevice);

    ENGINE_LOG("Preparing for rendering...");

    // Record a clear screen to the graphics command buffer
    cClearScreenRecorder clearRecorder(ppLightsRenderModule->GetRenderPass(), ppSwapChain);
    papCommandBuffers[0]->RecordBuffers(&clearRecorder);
    papCommandBuffers[1]->RecordBuffers(&clearRecorder);
    papCommandBuffers[2]->RecordBuffers(&clearRecorder);

    // Set up the game loop
    ppGameLoop = new cGameLoop();
    ppGameThread = new std::thread(std::ref(*ppGameLoop));

    ENGINE_LOG("Engine initialized");
}

void cEngine::ActivateOverlayWindow(const string& sName)
{
    auto tResult = pmOverlayWindows.find(sName);
    if (tResult == pmOverlayWindows.end())
    {
        throw std::runtime_error("Cannot find overlay window " + sName);
    }

    if (ppActiveOverlayWindow != nullptr)
    {
        ppGameLoop->RemoveTask(ppActiveOverlayWindow);
    }
    ppGameLoop->AddTask(tResult->second);

    ppRequestedOverlayWindow = tResult->second;
    pbUpdateOverlayWindow = true;
}

void cEngine::DeactivateOverlayWindow()
{
    if (ppActiveOverlayWindow != nullptr)
    {
        ppGameLoop->RemoveTask(ppActiveOverlayWindow);
    }
    ppRequestedOverlayWindow = nullptr;
    pbUpdateOverlayWindow = true;
}

cOverlayWindow* cEngine::GetActiveOverlayWindow()
{
    return ppActiveOverlayWindow;
}

void cEngine::MainLoop(void)
{
    ENGINE_LOG("Engine running");

    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        // Let the window do it's thing
        ppWindow->HandleEvents();

        // If the scene is signalling it wants to
        // quit, pass it on to the window
        if (ppScene != nullptr && ppScene->ShouldQuit())
        {
            ENGINE_LOG("Scene is asking for application quit");
            ppWindow->Close();
        }

        // If the active overlay is asking for the application
        // to quit, pass it on to the window
        if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldQuit())
        {
            ENGINE_LOG("Overlay is asking for application quit");
            ppWindow->Close();
        }

        if (ppWindow->ShouldRebuild())
        {
            ENGINE_LOG("Window is asking for pipeline rebuild");
            ppGameLoop->SetPaused(true);
            ppLogicalDevice->WaitUntilIdle();
            RebuildPipeline();
            ppGameLoop->SetPaused(false);
        }

        // Update the audio handler
        ppAudioHandler->Update();

        // Draw a frame
        ppRenderHandler->DrawFrame(ppScene, ppOverlayRenderModule, papCommandBuffers[2]);

        // If the scene hasn't been loaded, load it now
        // We want to draw at least one frame before loading the
        // scene, to allow loading text to be displayed
        if (!pbInitialized)
        {
            ENGINE_LOG("Loading overlay windows...");

            // Load the overlay windows
            LoadOverlayWindows(pmOverlayWindows);
            for (auto oOverlayWindow : pmOverlayWindows)
            {
                oOverlayWindow.second->Construct(ppTextureHandler, ppLogicalDevice,
                                                 this, ppOverlayRenderModule);
            }
            ENGINE_LOG("Loaded " << pmOverlayWindows.size() << " overlay windows");


            ENGINE_LOG("Loading scene...");

            // Ask the application for the scene
            LoadScene(&ppScene);

            // Create and load the scene
            ppScene->Load(ppTextureHandler, ppLogicalDevice, ppAudioHandler);

            // Setup the buffers for uniform variables
            ppLightsRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);
            ppMRTRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);
            ppOverlayRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);

            // We cannot (re-)record command buffers while the GPU is
            // using them, so we have to wait until it's idle.
            ppLogicalDevice->WaitUntilIdle();

            // Record the commands for rendering to the command buffer.
            cMRTRenderRecorder mrt(ppMRTRenderModule->GetRenderPass(), ppSwapChain,
                                   ppMRTRenderModule->GetRenderPipeline(),
                                   ppMRTRenderModule->GetUniformHandler(), ppScene);
            cLightingRenderRecorder light(ppLightsRenderModule->GetRenderPass(), ppSwapChain,
                                          ppLightsRenderModule->GetRenderPipeline(),
                                          ppLightsRenderModule->GetUniformHandler(), ppScene);
            papCommandBuffers[0]->RecordBuffers(&mrt);
            papCommandBuffers[1]->RecordBuffers(&light);

            ppOverlayRenderModule->CreateCommandRecorder();
            papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());

            ppAudioHandler->SetCamera(ppScene->GetCameraRef());

            ENGINE_LOG("Scene loaded, adding tick task...");
            ppScene->AfterLoad();
            ppGameLoop->AddTask(ppScene);
            ppGameLoop->SetPaused(false);

            pbInitialized = true;
        }
        else
        {
            for (auto&[sName, pWindow] : pmOverlayWindows)
            {
                pWindow->GetUIManager()->Update();
            }

            if (pbUpdateOverlayWindow)
            {
                ENGINE_LOG("Switching overlay window...");

                // Wait until the GPU is idle
                ppLogicalDevice->WaitUntilIdle();

                // Pause the game loop and optionally tell the scene to disable input
                ppGameLoop->SetPaused(true);
                if (ppRequestedOverlayWindow != nullptr && ppRequestedOverlayWindow->ShouldHandleInput())
                {
                    ppScene->OnInputDisable();
                }

                // Set the active window to the new one
                ppActiveOverlayWindow = ppRequestedOverlayWindow;

                // Setup the new uniform buffers
                ppOverlayRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, ppScene);
                // Re-record the command buffers
                papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());

                // Unpause the game loop
                ppGameLoop->SetPaused(false);

                // Clear the request variables
                ppRequestedOverlayWindow = nullptr;
                pbUpdateOverlayWindow = false;
            }
        }
    }

    // Stop the game loop before closing the main loop
    ppGameLoop->Stop();

    ENGINE_LOG("Main loop closed");

    // Wait until the logical device is idle before returning
    ppLogicalDevice->WaitUntilIdle();
}

void cEngine::Cleanup(void)
{
    ENGINE_LOG("Cleaning up engine...");

    delete ppAudioHandler;

    for (auto oOverlayWindow : pmOverlayWindows)
    {
        delete oOverlayWindow.second;
    }

    // Clean up the game thread
    ppGameThread->join();
    delete ppGameThread;
    delete ppGameLoop;

    delete ppScene;
    delete ppRenderHandler;
    delete ppOverlayRenderModule;
    delete ppTextureHandler;
    for (auto oBuffer : papCommandBuffers)
    {
        delete oBuffer;
    }
    ppLogicalDevice->DestroyCommandPool(cCommandHelper::poCommandPool, nullptr);
    delete ppMRTRenderModule;
    delete ppLightsRenderModule;
    delete ppSwapChain;
    delete ppLogicalDevice;
    ppWindow->DestroyWindowSurface(); // surface must be destroyed before the instance
    delete ppVulkanInstance;
    delete ppWindow;

    ENGINE_LOG("Engine shut down cleanly");
}

void cEngine::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleMouse(uiDeltaX, uiDeltaY);
    }
    else if (ppScene != nullptr)
    {
        ppScene->HandleMouse(uiDeltaX, uiDeltaY);
    }
}

void cEngine::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleKey(uiKeyCode, uiAction);
    }
    else if (ppScene != nullptr)
    {
        ppScene->HandleKey(uiKeyCode, uiAction);
    }
}

void cEngine::HandleScroll(double dOffsetX, double dOffsetY)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleScroll(dOffsetX, dOffsetY);
    }
    else if (ppScene != nullptr)
    {
        ppScene->HandleScroll(dOffsetX, dOffsetY);
    }
}

void cEngine::HandleCharacter(char cCharacter)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleCharacter(cCharacter);
    }
    else if (ppScene != nullptr)
    {
        ppScene->HandleCharacter(cCharacter);
    }
}

void cEngine::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }
    else if (ppScene != nullptr)
    {
        ppScene->HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }
}

cCommandBuffer** cEngine::GetCommandBuffers()
{
    return papCommandBuffers;
}

void cEngine::RebuildPipeline(void)
{
    ppSwapChain->RebuildSwapChain();

    ppSwapChain->CreateFramebuffers(ppLightsRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppMRTRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppOverlayRenderModule->GetRenderPass()->GetRenderPass());

    ppMRTRenderModule->GetRenderPipeline()->RebuildPipeline();
    ppLightsRenderModule->GetRenderPipeline()->RebuildPipeline();
    ppOverlayRenderModule->GetRenderPipeline()->RebuildPipeline();

    for (iUniformHandler* pUniformHandler : papUniformHandlers)
    {
        pUniformHandler->RebuildUniforms();
        pUniformHandler->SetupUniformBuffers(ppTextureHandler, ppScene);
    }

    // Record the commands for rendering to the command buffer.
    cMRTRenderRecorder mrt(ppMRTRenderModule->GetRenderPass(), ppSwapChain,
                           ppMRTRenderModule->GetRenderPipeline(),
                           ppMRTRenderModule->GetUniformHandler(), ppScene);
    cLightingRenderRecorder light(ppLightsRenderModule->GetRenderPass(), ppSwapChain,
                                  ppLightsRenderModule->GetRenderPipeline(),
                                  ppLightsRenderModule->GetUniformHandler(), ppScene);
    papCommandBuffers[0]->RecordBuffers(&mrt);
    papCommandBuffers[1]->RecordBuffers(&light);
    papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());
}
