#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/command/CommandBuffer.hpp>

class cRenderHandler
{
private:
    const uint uiMAX_FRAMES_IN_FLIGHT = 1;

    cLogicalDevice* ppLogicalDevice = nullptr;
    cSwapChain* ppSwapChain = nullptr;
    cCommandBuffer** ppCommandBuffers = nullptr;
    iUniformHandler** ppUniformHandlers = nullptr;

    uint puiUniformHandlerCount;
    std::vector<VkSemaphore> aoImageAvailableSemaphores;
    std::vector<VkSemaphore> aoMRTFinishedSemaphores;
    std::vector<VkSemaphore> aoRenderFinishedSemaphores;

    std::vector<VkFence> aoInFlightFences;

    uint uiCurrentFrame = 0;

public:
    cRenderHandler(cLogicalDevice* pLogicalDevice,
                   cSwapChain* pSwapChain,
                   cCommandBuffer** pCommandBuffers);
    ~cRenderHandler(void);

    void CreateSemaphores(void);

    void DrawFrame(cScene* pScene);

    void SetUniformHandlers(iUniformHandler** pUniformHandlers, uint uiUniformHandlerCount);
};

cRenderHandler::cRenderHandler(cLogicalDevice* pLogicalDevice, //-V730
                               cSwapChain* pSwapChain,
                               cCommandBuffer** pCommandBuffers)
{
    assert(pLogicalDevice != nullptr);
    assert(pSwapChain != nullptr);
    assert(pCommandBuffers != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppSwapChain = pSwapChain;
    ppCommandBuffers = pCommandBuffers;

    CreateSemaphores();
}

cRenderHandler::~cRenderHandler()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();
    for (uint i = 0; i < uiMAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(oDevice, aoRenderFinishedSemaphores[i], nullptr); //-V108
        vkDestroySemaphore(oDevice, aoMRTFinishedSemaphores[i], nullptr); //-V108
        vkDestroySemaphore(oDevice, aoImageAvailableSemaphores[i], nullptr); //-V108
        vkDestroyFence(oDevice, aoInFlightFences[i], nullptr); //-V108
    }
}

void cRenderHandler::CreateSemaphores()
{
    VkDevice& oDevice = ppLogicalDevice->GetDevice();

    // Resize all the semaphore & fence lists to the max amount of frames in flight
    aoImageAvailableSemaphores.resize(uiMAX_FRAMES_IN_FLIGHT); //-V106
    aoMRTFinishedSemaphores.resize(uiMAX_FRAMES_IN_FLIGHT); //-V106
    aoRenderFinishedSemaphores.resize(uiMAX_FRAMES_IN_FLIGHT); //-V106
    aoInFlightFences.resize(uiMAX_FRAMES_IN_FLIGHT); //-V106

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
        if (vkCreateSemaphore(oDevice, &tSemaphoreInfo, nullptr, &aoImageAvailableSemaphores[i]) != VK_SUCCESS ||//-V108
            vkCreateSemaphore(oDevice, &tSemaphoreInfo, nullptr, &aoMRTFinishedSemaphores[i]) != VK_SUCCESS || //-V108
            vkCreateSemaphore(oDevice, &tSemaphoreInfo, nullptr, &aoRenderFinishedSemaphores[i]) != VK_SUCCESS ||//-V108
            vkCreateFence(oDevice, &tFenceInfo, nullptr, &aoInFlightFences[i]) != VK_SUCCESS) //-V108
        {
            throw std::runtime_error("failed to create semaphores for a frame!");
        }
    }
}

void cRenderHandler::DrawFrame(cScene* pScene)
{
    static VkFence oNullFence = VK_NULL_HANDLE;

    // Wait for the fence of the current frame and reset it to the unsignalled state
    ppLogicalDevice->WaitForFences(1, &aoInFlightFences[uiCurrentFrame], VK_TRUE, UINT64_MAX); //-V108
    ppLogicalDevice->ResetFences(1, &aoInFlightFences[uiCurrentFrame]); //-V108

    // Acquire the next image from the swap chain
    uint uiImageIndex;
    ppSwapChain->AcquireNextImage(UINT64_MAX, aoImageAvailableSemaphores[uiCurrentFrame],
                                  oNullFence, &uiImageIndex); //-V108

    // If we have a loaded scene, update all the uniform handlers
    if (pScene != nullptr)
    {
        for (uint i = 0; i < puiUniformHandlerCount; i++)
        {
            ppUniformHandlers[i]->UpdateUniformBuffers(pScene); //-V108
        }
    }

    // Struct with information about the command buffer we want to submit to the queue
    VkSubmitInfo tSubmitInfo = {};
    tSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    // Specifies which semaphores to wait on and in which stage(s) of the pipeline to wait
    VkPipelineStageFlags aeWaitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    tSubmitInfo.waitSemaphoreCount = 1;
    tSubmitInfo.pWaitSemaphores = &aoImageAvailableSemaphores[uiCurrentFrame]; //-V108
    tSubmitInfo.pWaitDstStageMask = aeWaitStages;

    // Specify which command buffers to submit
    VkCommandBuffer aoBuffers[2] = {
            ppCommandBuffers[0]->GetBuffer(uiImageIndex),
            ppCommandBuffers[2]->GetBuffer(uiImageIndex)
    };

    tSubmitInfo.commandBufferCount = 2;
    tSubmitInfo.pCommandBuffers = aoBuffers;

    // Specify which semaphores to signal once the command buffer(s) finish
    tSubmitInfo.signalSemaphoreCount = 1;
    tSubmitInfo.pSignalSemaphores = &aoMRTFinishedSemaphores[uiCurrentFrame]; //-V108

    // Submit the command buffer to the queue
    if (!ppLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, oNullFence))
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Specifies which semaphores to wait on
    tSubmitInfo.pWaitSemaphores = &aoMRTFinishedSemaphores[uiCurrentFrame]; //-V108

    // Specify which command buffers to submit
    tSubmitInfo.commandBufferCount = 1;
    tSubmitInfo.pCommandBuffers = &ppCommandBuffers[1]->GetBuffer(uiImageIndex);

    // Specify which semaphores to signal once the command buffer(s) finish
    tSubmitInfo.signalSemaphoreCount = 1;
    tSubmitInfo.pSignalSemaphores = &aoRenderFinishedSemaphores[uiCurrentFrame]; //-V108

    // Submit the command buffer to the queue
    if (!ppLogicalDevice->GraphicsQueueSubmit(1, &tSubmitInfo, aoInFlightFences[uiCurrentFrame])) //-V108
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    // Struct with information for submitting the image for presentation
    VkPresentInfoKHR tPresentInfo = {};
    tPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Specify which semaphores to wait on before presenting
    tPresentInfo.waitSemaphoreCount = 1;
    tPresentInfo.pWaitSemaphores = &aoRenderFinishedSemaphores[uiCurrentFrame]; //-V108

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
