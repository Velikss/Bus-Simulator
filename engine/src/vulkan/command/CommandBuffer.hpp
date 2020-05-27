#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/swapchain/SwapChain.hpp>
#include <vulkan/command/CommandBufferRecorder.hpp>

class cCommandBuffer
{
private:
    cLogicalDevice* ppLogicalDevice;

    std::vector<VkCommandBuffer> paoCommandBuffers;

public:
    cCommandBuffer(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain);
    ~cCommandBuffer(void);

    void RecordBuffers(iCommandBufferRecorder* pRecorder);

    VkCommandBuffer& GetBuffer(uint uiIndex);

private:
    void CreateBuffers(cSwapChain* pSwapChain);
    void RecordBuffer(VkCommandBuffer& oBuffer,
                      iCommandBufferRecorder* pRecorder,
                      VkCommandBufferBeginInfo& tBeginInfo,
                      uint uiIndex);
};

cCommandBuffer::cCommandBuffer(cLogicalDevice* pLogicalDevice, cSwapChain* pSwapChain)
{
    ppLogicalDevice = pLogicalDevice;

    CreateBuffers(pSwapChain);
}

cCommandBuffer::~cCommandBuffer(void)
{
    ppLogicalDevice->FreeCommandBuffers(cCommandHelper::poCommandPool,
                                        (uint)paoCommandBuffers.size(),
                                        paoCommandBuffers.data());
}

void cCommandBuffer::CreateBuffers(cSwapChain* pSwapChain)
{
    // Make the command buffer list the same size as the framebuffer list
    paoCommandBuffers.resize(pSwapChain->GetFramebufferSize()); //-V106

    // Struct with information on how to allocate the command buffers
    VkCommandBufferAllocateInfo tAllocInfo = {};
    tAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    // Set the command pool
    tAllocInfo.commandPool = cCommandHelper::poCommandPool;

    // Specify if the allocated buffers should be primary or secondary command buffers
    tAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // Number of command buffers to allocate
    tAllocInfo.commandBufferCount = (uint) paoCommandBuffers.size();

    // Allocate the command buffers
    if (!ppLogicalDevice->AllocateCommandBuffers(&tAllocInfo, paoCommandBuffers.data()))
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void cCommandBuffer::RecordBuffers(iCommandBufferRecorder* pRecorder)
{
    // Struct with details about the usage of this buffer
    VkCommandBufferBeginInfo tBeginInfo = {};
    tBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    tBeginInfo.flags = 0;
    tBeginInfo.pInheritanceInfo = nullptr;

    // Loop over all the buffers and record the commands using the recorder
    for (uint i = 0; i < paoCommandBuffers.size(); i++) //-V104
    {
        RecordBuffer(paoCommandBuffers[i], pRecorder, tBeginInfo, i); //-V108
    }
}

void cCommandBuffer::RecordBuffer(VkCommandBuffer& oBuffer,
                                  iCommandBufferRecorder* pRecorder,
                                  VkCommandBufferBeginInfo& tBeginInfo,
                                  uint uiIndex)
{
    // Setup the recorder
    pRecorder->Setup(uiIndex);

    // Begin recording the command buffer
    if (vkBeginCommandBuffer(oBuffer, &tBeginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Record the commands
    pRecorder->RecordCommands(oBuffer, uiIndex);

    // End recording to the command buffer
    if (vkEndCommandBuffer(oBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }
}

VkCommandBuffer& cCommandBuffer::GetBuffer(uint uiIndex)
{
    return paoCommandBuffers[uiIndex]; //-V108
}
