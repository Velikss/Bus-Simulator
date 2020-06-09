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
#include <vulkan/util/GameManager.hpp>
#include <vulkan/util/CommandBufferHolder.hpp>
#include <vulkan/SceneManager.hpp>
#include <vulkan/util/Settings.hpp>
#include <vulkan/util/Profiler.hpp>
#include <vulkan/geometry/GeometryHandler.hpp>

class cEngine : public iGameManager, public iInputHandler, public iCommandBufferHolder
{
private:
    const string psAppName;

    // Application window instance
    cWindow* ppWindow = nullptr;
    // Vulkan API instance
    cVulkanInstance* ppVulkanInstance = nullptr;

    // Logical representation of the graphical device
    cLogicalDevice* ppLogicalDevice = nullptr;
    // Swap chain for managing frame buffers
    cSwapChain* ppSwapChain = nullptr;
    // Texture handler for loading and managing textures
    cTextureHandler* ppTextureHandler = nullptr;
    cGeometryHandler* ppGeometryHandler = nullptr;

    // Render handler for handling frame drawing logic
    cRenderHandler* ppRenderHandler = nullptr;

    // Render modules for MRT (multi render target)
    // overlay and lighting
    // Render modules manage the uniforms, graphics
    // pipeline, and render pass
    cMRTRenderModule* ppMRTRenderModule = nullptr;
    cOverlayRenderModule* ppOverlayRenderModule = nullptr;
    cRenderModule* ppLightsRenderModule = nullptr;

    // Command buffers for all render modules
    cCommandBuffer* papCommandBuffers[3];
    // Uniform handlers for all render modules
    iUniformHandler* papUniformHandlers[3];

    // Gameloop and thread for the gameloop
    cGameLoop* ppGameLoop = nullptr;
    std::thread* ppGameThread = nullptr;

    // Scene manager for storing and loading scenes
    cSceneManager* ppSceneManager = nullptr;
    // Audio handler for playing audio
    cAudioHandler* ppAudioHandler = nullptr;

    // Map with all loaded overlay windows
    std::map<string, cOverlayWindow*> pmOverlayWindows;
    // Current active overlay window
    cOverlayWindow* ppActiveOverlayWindow = nullptr;

    // Variables for requesting an overlay window change
    cOverlayWindow* ppRequestedOverlayWindow = nullptr;
    bool pbUpdateOverlayWindow = false;
    // Variables for requesting a scene change
    string psRequestedScene = "";
    bool pbUpdateScene = false;

    // Property indicating if the engine is fully initialized
    bool pbInitialized = false;

public:
    cEngine(const string& sAppName = "Game");

    // Initializes and starts the engine and all of it's sub-components
    void Run(void);

    // Get the currently active overlay window
    // Can be null if no window is active
    cOverlayWindow* GetActiveOverlayWindow() override;
    // Request a certain overlay window to be activated
    void ActivateOverlayWindow(const string& sName) override;
    // Returns overlay by name
    cOverlayWindow* GetOverlayByName(const string& sName) override;
    // Request the active overlay window to be deactivated
    void DeactivateOverlayWindow() override;
    // Request a scene switch
    void SwitchScene(const string& sName) override;
    // Get all the scenes
    std::map<string, cScene*>& GetScenes() override;

    // Get an array with the command buffers
    // TODO: Remove this method
    cCommandBuffer** GetCommandBuffers() override;

    // Input handling
    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;
    void HandleScroll(double dOffsetX, double dOffsetY) override;
    void HandleCharacter(char cCharacter) override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

protected:
    // Shader loading from the application
    virtual void LoadMRTShaders(std::vector<string>& shaders) = 0;
    virtual void LoadLightingShaders(std::vector<string>& shaders) = 0;
    virtual void LoadOverlayShaders(std::vector<string>& shaders) = 0;

    // Overlay and scene loading from the application
    virtual void LoadOverlayWindows(std::map<string, cOverlayWindow*>& mOverlayWindows) = 0;
    virtual void SetupScenes(std::map<string, cScene*>& mScenes, string* sInitialScene) = 0;

private:
    // Initialization
    void InitWindow(void);
    void InitAudio(void);
    void InitVulkan(void);
    void InitEngine(void);
    // Engine main loop
    void MainLoop(void);
    // Engine cleanup
    void Cleanup(void);

    // Switch the active overlay window
    void UpdateOverlay(void);
    // Switch the active scene
    void UpdateScene(void);

    // Rebuild the entire pipeline
    void RebuildPipeline(void);
    // (Re-)record the command buffers
    void RecordCommandBuffers(void);
    // Rebuild the uniforms
    void RebuildUniforms(void);
};

cEngine::cEngine(const string& sAppName) : psAppName(sAppName)
{
}

void cEngine::Run()
{
    // Initialize everything
    InitWindow();
    InitAudio();
    InitVulkan();
    InitEngine();
    // Run the main loop
    MainLoop();
    // Clean up the engine
    Cleanup();
}

void cEngine::InitWindow(void)
{
    // Create the window and set us as the input handler
    ppWindow = new cWindow(psAppName);
    ppWindow->CreateGLWindow();
    ppWindow->ppInputHandler = this;
}

void cEngine::InitAudio(void)
{
    ENGINE_LOG("Initializing audio system...");

    // Create a new audio handler
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

    ppGeometryHandler = new cGeometryHandler(ppLogicalDevice);

    ENGINE_LOG("Preparing for rendering...");

    // Record a clear screen to the graphics command buffer
    cClearScreenRecorder clearRecorder(ppLightsRenderModule->GetRenderPass(), ppSwapChain);
    papCommandBuffers[0]->RecordBuffers(&clearRecorder);
    papCommandBuffers[1]->RecordBuffers(&clearRecorder);
    papCommandBuffers[2]->RecordBuffers(&clearRecorder);
}

void cEngine::InitEngine(void)
{
    // Create the scene manager
    ppSceneManager = new cSceneManager(ppLogicalDevice, ppTextureHandler, ppGeometryHandler, ppAudioHandler);

    cSettings::Init();

    // Set up the game loop
    ppGameLoop = new cGameLoop();
    ppGameThread = new std::thread(std::ref(*ppGameLoop));

    ENGINE_LOG("Engine initialized");
}

void cEngine::ActivateOverlayWindow(const string& sName)
{
    // Try to find an overlay window with the given name
    auto tResult = pmOverlayWindows.find(sName);
    if (tResult == pmOverlayWindows.end())
    {
        throw std::runtime_error("Cannot find overlay window " + sName);
    }

    // If there currently is an overlay window active, remove it's tick task
    if (ppActiveOverlayWindow != nullptr)
    {
        ppActiveOverlayWindow->OnClose();
        ppGameLoop->RemoveTask(ppActiveOverlayWindow);
    }
    // Add a tick task for the new overlay window
    ppGameLoop->AddTask(tResult->second);

    // Request an overlay window change
    ppRequestedOverlayWindow = tResult->second;
    pbUpdateOverlayWindow = true;
}

void cEngine::DeactivateOverlayWindow()
{
    // If an overlay window is active, request an overlay window update
    if (ppActiveOverlayWindow != nullptr)
    {
        ppActiveOverlayWindow->OnClose();
        ppGameLoop->RemoveTask(ppActiveOverlayWindow);
        ppRequestedOverlayWindow = nullptr;
        pbUpdateOverlayWindow = true;
    }
}

cOverlayWindow* cEngine::GetActiveOverlayWindow()
{
    return ppActiveOverlayWindow;
}

void cEngine::SwitchScene(const string& sName)
{
    // Request a scene switch
    psRequestedScene = sName;
    pbUpdateScene = true;
}

void cEngine::MainLoop(void)
{
    ENGINE_LOG("Engine running");

    // Keep the main loop running until the window should be closed
    while (!ppWindow->ShouldClose())
    {
        // Let the window do it's thing
        ppWindow->HandleEvents();

        // Get the active scene
        cScene* pScene = ppSceneManager->GetActiveScene();

        // If the scene is signalling it wants to
        // quit, pass it on to the window
        if (pScene != nullptr && pScene->ShouldQuit())
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

        // If the window wants a rebuild, rebuild the pipeline
        // To rebuild, pause the gameloop, wait until the GPU
        // is idle, rebuild the pipeline, and unpause the gameloop
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
        ppRenderHandler->DrawFrame(pScene);

        // Run the profiler
        cProfiler::poInstance.Update();

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

            ENGINE_LOG("Loading scene...");

            // Ask the application for the scenes
            string sActiveScene = "";
            SetupScenes(ppSceneManager->GetScenes(), &sActiveScene);
            assert(sActiveScene.size() > 0);

            // Load the scene
            ppSceneManager->SwitchScene(sActiveScene);
            pScene = ppSceneManager->GetActiveScene();
            assert(pScene != nullptr);

            // Setup the buffers for uniform variables
            ppLightsRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, pScene);
            ppMRTRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, pScene);
            ppOverlayRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, pScene);

            // We cannot (re-)record command buffers while the GPU is
            // using them, so we have to wait until it's idle.
            ppLogicalDevice->WaitUntilIdle();

            // Record the commands for rendering to the command buffer.
            ppOverlayRenderModule->CreateCommandRecorder();
            RecordCommandBuffers();

            // Pass the camera reference to the audio handler
            ppAudioHandler->SetCamera(pScene->GetCameraRef());

            ENGINE_LOG("Scene loaded, adding tick task...");
            pScene->AfterLoad();
            ppGameLoop->AddTask(pScene);

            // Unpause the gameloop
            ppGameLoop->SetPaused(false);

            pbInitialized = true;
        }
        else if (pScene != nullptr)
        {
            // Update the UI manager
            for (auto&[sName, pWindow] : pmOverlayWindows)
            {
                pWindow->GetUIManager()->Update();
            }

            // Update the overlay window if requested
            if (pbUpdateOverlayWindow)
            {
                UpdateOverlay();
                pbUpdateOverlayWindow = false;
            }
            else if (pbUpdateScene) // Update the scene if requested
            {
                UpdateScene();
                pbUpdateScene = false;
            }
            else
            {
                // Handle mesh texture updates
                cMRTUniformHandler* pUniformHandler = ((cMRTUniformHandler*) ppMRTRenderModule->GetUniformHandler());
                if (pUniformHandler->UpdateUniformTextures(pScene))
                {
                    RecordCommandBuffers();
                }
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

    delete ppSceneManager;
    delete ppRenderHandler;
    delete ppOverlayRenderModule;
    delete ppTextureHandler;
    delete ppGeometryHandler;
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

void cEngine::UpdateOverlay(void)
{
    ENGINE_LOG("Switching overlay window...");

    // Get the active scene
    cScene* pScene = ppSceneManager->GetActiveScene();
    assert(pScene != nullptr);

    // Wait until the GPU is idle
    ppLogicalDevice->WaitUntilIdle();

    // Pause the gameloop
    ppGameLoop->SetPaused(true);

    // If the overlay window wants to handle input, disable scene input handling
    if (ppRequestedOverlayWindow != nullptr && ppRequestedOverlayWindow->ShouldHandleInput())
    {
        pScene->OnInputDisable();
    }

    // Set the active window to the new one
    ppActiveOverlayWindow = ppRequestedOverlayWindow;

    // Setup the new uniform buffers for the overlay
    ppOverlayRenderModule->GetUniformHandler()->SetupUniformBuffers(ppTextureHandler, pScene);
    // Re-record the command buffers for the overlay
    papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());

    // Unpause the game loop
    ppGameLoop->SetPaused(false);

    // Clear the request variables
    ppRequestedOverlayWindow = nullptr;

    if (ppActiveOverlayWindow != nullptr)
    {
        ppActiveOverlayWindow->OnOpen();
    }
}

void cEngine::UpdateScene(void)
{
    ENGINE_LOG("Switching active scene...");

    // Get the active scene
    cScene* pScene = ppSceneManager->GetActiveScene();
    assert(pScene != nullptr);

    // Pause the gameloop and remove the tick task for the current scene
    ppGameLoop->SetPaused(true);
    ppGameLoop->RemoveTask(pScene);

    // Wait until the GPU is idle
    ppLogicalDevice->WaitUntilIdle();

    // Tell the scene manager to switch to the requested scene
    // This will automatically trigger a load on the new scene
    ppSceneManager->SwitchScene(psRequestedScene);
    pScene = ppSceneManager->GetActiveScene();
    assert(pScene != nullptr);

    // Rebuild uniforms and command buffers
    RebuildUniforms();
    RecordCommandBuffers();

    // Close the active overlay window
    DeactivateOverlayWindow();

    // Get the scene ready and add a tick task for it
    ppAudioHandler->SetCamera(pScene->GetCameraRef());
    pScene->AfterLoad();
    ppGameLoop->AddTask(pScene);

    // Unpause the gameloop
    ppGameLoop->SetPaused(false);
    psRequestedScene = "";
}

void cEngine::RebuildPipeline(void)
{
    // Rebuild the swap chain
    ppSwapChain->RebuildSwapChain();

    ppMRTRenderModule->RebuildRenderPass();

    fSleep(100);

    // Recreate the framebuffer for all required resources
    ppSwapChain->CreateFramebuffers(ppLightsRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppMRTRenderModule->GetRenderPass()->GetRenderPass(),
                                    ppOverlayRenderModule->GetRenderPass()->GetRenderPass());

    // Rebuild all the pipelines
    ppMRTRenderModule->GetRenderPipeline()->RebuildPipeline(ppMRTRenderModule->GetRenderPass());
    ppLightsRenderModule->GetRenderPipeline()->RebuildPipeline(ppLightsRenderModule->GetRenderPass());
    ppOverlayRenderModule->GetRenderPipeline()->RebuildPipeline(ppOverlayRenderModule->GetRenderPass());

    // Rebuild uniforms and command buffers
    RebuildUniforms();
    RecordCommandBuffers();
}

void cEngine::RecordCommandBuffers(void)
{
    // Get the active scene
    cScene* pScene = ppSceneManager->GetActiveScene();
    assert(pScene != nullptr);

    // Record the commands for rendering to the command buffer.
    cMRTRenderRecorder mrt(ppMRTRenderModule->GetRenderPass(), ppSwapChain,
                           ppMRTRenderModule->GetRenderPipeline(),
                           ppMRTRenderModule->GetUniformHandler(), pScene);
    cLightingRenderRecorder light(ppLightsRenderModule->GetRenderPass(), ppSwapChain,
                                  ppLightsRenderModule->GetRenderPipeline(),
                                  ppLightsRenderModule->GetUniformHandler(), pScene);
    papCommandBuffers[0]->RecordBuffers(&mrt);
    papCommandBuffers[1]->RecordBuffers(&light);
    papCommandBuffers[2]->RecordBuffers(ppOverlayRenderModule->GetCommandRecorder());
}

void cEngine::RebuildUniforms(void)
{
    // Get the active scene
    cScene* pScene = ppSceneManager->GetActiveScene();
    assert(pScene != nullptr);

    // Loop over all uniform handlers and call a rebuild
    for (iUniformHandler* pUniformHandler : papUniformHandlers)
    {
        pUniformHandler->RebuildUniforms();
        pUniformHandler->SetupUniformBuffers(ppTextureHandler, pScene);
    }
}

cCommandBuffer** cEngine::GetCommandBuffers()
{
    return papCommandBuffers;
}

std::map<string, cScene*>& cEngine::GetScenes()
{
    return ppSceneManager->GetScenes();
}

void cEngine::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleMouse(uiDeltaX, uiDeltaY);
    }
    else if (ppSceneManager->GetActiveScene() != nullptr)
    {
        ppSceneManager->GetActiveScene()->HandleMouse(uiDeltaX, uiDeltaY);
    }
}

void cEngine::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleKey(uiKeyCode, uiAction);
    }
    else if (ppSceneManager->GetActiveScene() != nullptr)
    {
        ppSceneManager->GetActiveScene()->HandleKey(uiKeyCode, uiAction);
    }
}

void cEngine::HandleScroll(double dOffsetX, double dOffsetY)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleScroll(dOffsetX, dOffsetY);
    }
    else if (ppSceneManager->GetActiveScene() != nullptr)
    {
        ppSceneManager->GetActiveScene()->HandleScroll(dOffsetX, dOffsetY);
    }
}

void cEngine::HandleCharacter(char cCharacter)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleCharacter(cCharacter);
    }
    else if (ppSceneManager->GetActiveScene() != nullptr)
    {
        ppSceneManager->GetActiveScene()->HandleCharacter(cCharacter);
    }
}

void cEngine::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    if (ppActiveOverlayWindow != nullptr && ppActiveOverlayWindow->ShouldHandleInput())
    {
        ppActiveOverlayWindow->HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }
    else if (ppSceneManager->GetActiveScene() != nullptr)
    {
        ppSceneManager->GetActiveScene()->HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }
}

cOverlayWindow* cEngine::GetOverlayByName(const string& sName)
{
    auto tResult = pmOverlayWindows.find(sName);
    if (tResult == pmOverlayWindows.end()) return nullptr;
    return tResult->second;
}
