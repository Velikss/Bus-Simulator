#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/overlay/TextHandler.hpp>
#include "SwapChain.hpp"
#include "vulkan/command/CommandBuffer.hpp"

class cRenderHandler
{
private:
    const uint uiMAX_FRAMES_IN_FLIGHT = 2;

    cLogicalDevice* ppLogicalDevice;
    cSwapChain* ppSwapChain;
    cCommandBuffer** ppCommandBuffers;
    uint puiCommandBufferCount;
    iUniformHandler** ppUniformHandlers = nullptr;
    uint puiUniformHandlerCount;

    std::vector<VkSemaphore> aoImageAvailableSemaphores;
    std::vector<VkSemaphore> aoRenderFinishedSemaphores;
    std::vector<VkFence> aoInFlightFences;

    uint uiCurrentFrame = 0;

public:
    cRenderHandler(cLogicalDevice* pLogicalDevice,
                   cSwapChain* pSwapChain,
                   cCommandBuffer** pCommandBuffers,
                   uint uiCommandBufferCount);
    ~cRenderHandler(void);

    void CreateSemaphores(void);

    void DrawFrame(cScene* pScene, cTextHandler* pTextHandler, cCommandBuffer* pCommandBuffer);

    void SetUniformHandlers(iUniformHandler** pUniformHandlers, uint uiUniformHandlerCount);
};

cRenderHandler::cRenderHandler(cLogicalDevice* pLogicalDevice,
                               cSwapChain* pSwapChain,
                               cCommandBuffer** pCommandBuffers,
                               uint uiCommandBufferCount)
{
    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;
    ppCommandBuffers = pCommandBuffers;
    puiCommandBufferCount = uiCommandBufferCount;

    CreateSemaphores();
}

cRenderHandler::~cRenderHandler()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();
    for (uint i = 0; i < uiMAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(oDevice, aoRenderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(oDevice, aoImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(oDevice, aoInFlightFences[i], nullptr);
    }
}

void cRenderHandler::CreateSemaphores()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();

    // Resize all the semaphore & fence lists to the max amount of frames in flight
    aoImageAvailableSemaphores.resize(uiMAX_FRAMES_IN_FLIGHT);
    aoRenderFinishedSemaphores.resize(uiMAX_FRAMES_IN_FLIGHT);
    aoInFlightFences.resize(uiMAX_FRAMES_IN_FLIGHT);

    // Struct with information about the semaphores
    VkSemaphoreCreateInfo tSemaphoreInfo = {};
    tSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Struct with information about the fences
    VkFenceCreateInfo tFenceInfo = {};
    tFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    tFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint i = 0; i < uiMAX_FRAMES_IN_FLIGHT; i++)
    {
        // For every frame, create the two semaphores and the fence
        if (vkCreateSemaphore(oDevice, &tSemaphoreInfo, nullptr, &aoImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(oDevice, &tSemaphoreInfo, nullptr, &aoRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(oDevice, &tFenceInfo, nullptr, &aoInFlightFences[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create semaphores for a frame!");
        }
    }
}

void cRenderHandler::DrawFrame(cScene* pScene, cTextHandler* pTextHandler, cCommandBuffer* pCommandBuffer)
{
#ifdef ENABLE_FPS_COUNT
    static auto startTime = std::chrono::high_resolution_clock::now();
    static uint frameCount = 0;

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    frameCount++;
    if (time >= 1)
    {
        ppLogicalDevice->WaitUntilIdle();
        static uint test = 0;
        char text[64];
        sprintf(text, "%u fps", frameCount);
        pTextHandler->UpdateText(text);
        pCommandBuffer->RecordBuffers(pTextHandler->GetCommandRecorder());
        startTime = currentTime;
        frameCount = 0;
    }
#endif

    // Wait for the fence of the current frame and reset it to the unsignalled state
    ppLogicalDevice->WaitForFences(1, &aoInFlightFences[uiCurrentFrame], VK_TRUE, UINT64_MAX);
    ppLogicalDevice->ResetFences(1, &aoInFlightFences[uiCurrentFrame]);

    // Acquire the next image from the swap chain
    uint uiImageIndex;
    VkFence oAqcuireFence = VK_NULL_HANDLE;
    ppSwapChain->AcquireNextImage(UINT64_MAX, aoImageAvailableSemaphores[uiCurrentFrame], oAqcuireFence, &uiImageIndex);

    for (uint i = 0; i < puiUniformHandlerCount; i++)
    {
        ppUniformHandlers[i]->UpdateUniformBuffers(pScene);
    }

    // Struct with information about the command buffer we want to submit to the queue
    VkSubmitInfo tSubmitInfo = {};
    tSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Specifies which semaphores to wait on and in which stage(s) of the pipeline to wait
    VkSemaphore aoWaitSemaphores[] = {aoImageAvailableSemaphores[uiCurrentFrame]};
    VkPipelineStageFlags aeWaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    tSubmitInfo.waitSemaphoreCount = 1;
    tSubmitInfo.pWaitSemaphores = aoWaitSemaphores;
    tSubmitInfo.pWaitDstStageMask = aeWaitStages;

    // Specify which command buffers to submit
    std::vector<VkCommandBuffer> aBuffers;
    for (uint i = 0; i < puiCommandBufferCount; i++)
    {
        aBuffers.push_back(ppCommandBuffers[i]->GetBuffer(uiImageIndex));
    }

    tSubmitInfo.commandBufferCount = aBuffers.size();
    tSubmitInfo.pCommandBuffers = aBuffers.data();

    // Specify which semaphores to signal once the command buffer(s) finish
    VkSemaphore aoSignalSemaphores[] = {aoRenderFinishedSemaphores[uiCurrentFrame]};
    tSubmitInfo.signalSemaphoreCount = 1;
    tSubmitInfo.pSignalSemaphores = aoSignalSemaphores;

    // Submit the command buffer to the queue
    if (!ppLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, aoInFlightFences[uiCurrentFrame]))
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Struct with information for submitting the image for presentation
    VkPresentInfoKHR tPresentInfo = {};
    tPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Specify which semaphores to wait on before presenting
    tPresentInfo.waitSemaphoreCount = 1;
    tPresentInfo.pWaitSemaphores = aoSignalSemaphores;

    // Specify the swap chains and the index of the image for each swap chain
    VkSwapchainKHR swapChains[] = {ppSwapChain->poSwapChain};
    tPresentInfo.swapchainCount = 1;
    tPresentInfo.pSwapchains = swapChains;
    tPresentInfo.pImageIndices = &uiImageIndex;

    // Optional, allows you to get a result for every individual swap chain presentation
    tPresentInfo.pResults = nullptr;

    // Queue the image for presentation
    ppLogicalDevice->QueuePresent(&tPresentInfo);

    // Advance to the next frame
    uiCurrentFrame = (uiCurrentFrame + 1) % uiMAX_FRAMES_IN_FLIGHT;
}

void cRenderHandler::SetUniformHandlers(iUniformHandler** pUniformHandlers, uint uiUniformHandlerCount)
{
    ppUniformHandlers = pUniformHandlers;
    puiUniformHandlerCount = uiUniformHandlerCount;
}
